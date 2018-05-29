#include <oni/messaging/messagemanager.h>
#include <oni/messaging/messagecategory.h>
#include <oni/messaging/message.h>
#include <oni/utils/sys_wrappers.h>
#include <oni/utils/memory/allocator.h>
#include <oni/utils/logger.h>

void __dec(struct allocation_t* allocation);

void messagemanager_init(struct messagemanager_t* manager)
{
	if (!manager)
		return;

	spin_init(&manager->lock);

	kmemset(manager->categories, 0, sizeof(manager->categories));
}

int32_t messagemanager_findFreeCategoryIndex(struct messagemanager_t* manager)
{
	if (!manager)
		return -1;

	int32_t result = -1;

	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < RPCDISPATCHER_MAX_CATEGORIES; ++i)
	{
		if (!manager->categories[i])
		{
			result = i;
			break;
		}
	}
	spin_unlock(&manager->lock);

	return result;
}

uint32_t messagemanager_freeCategoryCount(struct messagemanager_t* manager)
{
	if (!manager)
		return 0;

	uint32_t clientCount = 0;
	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < RPCDISPATCHER_MAX_CATEGORIES; ++i)
	{
		if (manager->categories[i])
			clientCount++;
	}
	spin_unlock(&manager->lock);

	return clientCount;
}

struct messagecategory_t* messagemanager_getCategory(struct messagemanager_t* manager, uint32_t category)
{
	if (!manager)
		return 0;

	//WriteLog(LL_Debug, "[+] got dispatcher %p category: %d", manager, category);

	if (category >= RPCCAT_COUNT)
		return 0;

	struct messagecategory_t* rpccategory = 0;

	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < RPCDISPATCHER_MAX_CATEGORIES; ++i)
	{
		struct messagecategory_t* cat = manager->categories[i];

		if (!cat)
			continue;

		if (cat->category == category)
		{
			rpccategory = manager->categories[i];
			break;
		}
	}
	spin_unlock(&manager->lock);

	return rpccategory;
}

int32_t messagemanager_registerCallback(struct messagemanager_t* manager, uint32_t callbackCategory, uint32_t callbackType, void* callback)
{
	// Verify that the dispatcher and listener are valid
	if (!manager || !callback)
		return 0;

	// Verify the listener category
	if (callbackCategory >= RPCCAT_COUNT)
		return 0;

	struct messagecategory_t* category = messagemanager_getCategory(manager, callbackCategory);
	if (!category)
	{
		// Get a free listener index
		int32_t freeIndex = messagemanager_findFreeCategoryIndex(manager);
		if (freeIndex == -1)
			return 0;

		// Allocate a new category
		category = (struct messagecategory_t*)kmalloc(sizeof(struct messagecategory_t));
		if (!category)
			return 0;

		// Initialize the category
		rpccategory_init(category, callbackCategory);

		// Set the category in our list
		manager->categories[freeIndex] = category;
	}

	// Get the next free listener
	int32_t callbackIndex = rpccategory_findFreeCallbackIndex(category);
	if (callbackIndex == -1)
		return 0;

	// Install the listener to the category
	//category->callbacks[callbackIndex] = callback;
	struct messagecategory_callback_t* categoryCallback = (struct messagecategory_callback_t*)kmalloc(sizeof(struct messagecategory_callback_t));
	if (!categoryCallback)
		return 0;

	categoryCallback->type = callbackType; // TODO: Set type
	categoryCallback->callback = callback;

	category->callbacks[callbackIndex] = categoryCallback;

	return 1;
}

int32_t messagemanager_unregisterCallback(struct messagemanager_t* manager, int32_t callbackCategory, int32_t callbackType, void* callback)
{
	// Verify that the dispatcher and listener are valid
	if (!manager || !callback)
		return false;

	// Verify the listener category
	if (callbackCategory >= RPCCAT_COUNT)
		return false;

	struct messagecategory_t* category = messagemanager_getCategory(manager, callbackCategory);
	if (!category)
		return false;

	for (uint32_t l_CallbackIndex = 0; l_CallbackIndex < ARRAYSIZE(category->callbacks); ++l_CallbackIndex)
	{
		// Get the category callback structure
		struct messagecategory_callback_t* l_Callback = category->callbacks[l_CallbackIndex];

		// Check to see if this callback is used at all
		if (!l_Callback)
			continue;

		// Check the type of the message
		if (l_Callback->type != callbackType)
			continue;

		// Check the callback
		if (l_Callback->callback != callback)
			continue;

		// Remove from the list
		category->callbacks[l_CallbackIndex] = NULL;

		// Free the memory
		kfree(l_Callback, sizeof(*l_Callback));

		l_Callback = NULL;

		return true;
	}

	return false;
}

void messagemanager_sendMessage(struct messagemanager_t* manager, struct allocation_t* msg)
{
	if (!manager || !msg)
		return;

	struct message_t* message = __get(msg);
	if (!message)
	{
		WriteLog(LL_Error, "could not get reference to message");
		return; // On initial get, don't jump to cleanup
	}

	// We forward all replies
	if (message->header.request == false && message->socket != -1)
	{
		WriteLog(LL_Debug, "[+] Sending network response back to PC");
		kwrite(message->socket, &message->header, sizeof(message->header));

		if (message->header.payloadSize > 0 && message->payload != NULL && message->header.error_type == 0)
			kwrite(message->socket, message->payload, message->header.payloadSize);

		goto cleanup;
	}

	//WriteLog(LL_Debug, "[+] sendMessage dispatcher: %p ref: %p message: %p", manager, msg, message);

	if (message->header.category >= RPCCAT_COUNT)
	{
		WriteLog(LL_Error, "[-] invalid message category: %d max: %d", message->header.category, RPCCAT_COUNT);
		goto cleanup;
	}

	//WriteLog(LL_Debug, "[+] getting dispatcher category");

	struct messagecategory_t* category = messagemanager_getCategory(manager, message->header.category);
	if (!category)
	{
		WriteLog(LL_Debug, "[-] could not get dispatcher category");
		goto cleanup;
	}

	// Iterate through all of the callbacks
	//WriteLog(LL_Debug, "[+] iterating callbacks");
	for (uint32_t l_CallbackIndex = 0; l_CallbackIndex < RPCCATEGORY_MAX_CALLBACKS; ++l_CallbackIndex)
	{
		// Get the category callback structure
		struct messagecategory_callback_t* l_Callback = category->callbacks[l_CallbackIndex];

		// Check to see if this callback is used at all
		if (!l_Callback)
			continue;

		// Check the type of the message
		if (l_Callback->type != message->header.error_type)
			continue;

		// Call the callback with the provided message
		WriteLog(LL_Debug, "[+] calling callback %p(%p)", l_Callback->callback, msg);
		l_Callback->callback(msg);
	}

cleanup:
	__dec(msg);
}

void messagemanager_sendSuccessMessage(struct messagemanager_t* manager, struct allocation_t* msg)
{
	if (!manager || !msg)
		return;

	struct message_t* message = __get(msg);
	if (!message)
	{
		WriteLog(LL_Error, "could not get reference to message");
		return;
	}

	int request = message->header.request;
	message->header.request = false;

	// We forward all replies
	if (message->socket < 0)
		goto cleanup;

	// Set success error
	message->header.error_type = 0;

	//WriteLog(LL_Debug, "sending success network response back to PC");

	// Save the payload length
	uint16_t payloadLength = message->header.payloadSize;

	// Set set the payload length to 0 because we are not sending a payload
	message->header.payloadSize = 0;

	// Send response back to the PC
	kwrite(message->socket, &message->header, sizeof(message->header));

	// Set the payload length back so memory cleanup will happen normally
	message->header.payloadSize = payloadLength;

cleanup:
	message->header.request = request;
	__dec(msg);
}

void messagemanager_sendErrorMessage(struct messagemanager_t* manager, struct allocation_t* msg, int32_t error)
{
	if (!manager || !msg)
		return;

	struct message_t* message = __get(msg);
	if (!message)
	{
		WriteLog(LL_Error, "could not get reference to message");
		return;
	}
	int request = message->header.request;
	message->header.request = false;

	// We forward all replies
	if (message->socket != -1)
	{
		// Set success error
		message->header.error_type = 0;

		//WriteLog(LL_Debug, "sending success network response back to PC");

		// Save the payload length
		uint16_t payloadLength = message->header.payloadSize;

		// Set set the payload length to 0 because we are not sending a payload
		message->header.payloadSize = 0;

		// The error should always be < 0
		message->header.error_type = error < 0 ? error : -error;

		// Send response back to the PC
		kwrite(message->socket, &message->header, sizeof(message->header));

		// Set the payload length back so memory cleanup will happen normally
		message->header.payloadSize = payloadLength;
	}

	message->header.request = request;

	__dec(msg);
}