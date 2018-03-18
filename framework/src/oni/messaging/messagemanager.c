



#include <oni/messaging/messagemanager.h>
#include <oni/messaging/messagecategory.h>
#include <oni/messaging/message.h>
#include <oni/utils/sys_wrappers.h>
#include <oni/utils/memory/allocator.h>
#include <oni/utils/log/logger.h>

void __dec(struct allocation_t* allocation);

void messagemanager_init(struct messagemanager_t* dispatcher)
{
	if (!dispatcher)
		return;

	dispatcher->lock.lock = 0;

	kmemset(dispatcher->categories, 0, sizeof(dispatcher->categories));
}

int32_t messagemanager_findFreeCategoryIndex(struct messagemanager_t* dispatcher)
{
	if (!dispatcher)
		return -1;

	int32_t result = -1;

	spin_lock(&dispatcher->lock);
	for (uint32_t i = 0; i < RPCDISPATCHER_MAX_CATEGORIES; ++i)
	{
		if (!dispatcher->categories[i])
		{
			result = i;
			break;
		}
	}
	spin_unlock(&dispatcher->lock);

	return result;
}

uint32_t messagemanager_freeCategoryCount(struct messagemanager_t* dispatcher)
{
	if (!dispatcher)
		return 0;

	uint32_t clientCount = 0;
	spin_lock(&dispatcher->lock);
	for (uint32_t i = 0; i < RPCDISPATCHER_MAX_CATEGORIES; ++i)
	{
		if (dispatcher->categories[i])
			clientCount++;
	}
	spin_unlock(&dispatcher->lock);

	return clientCount;
}

struct messagecategory_t* messagemanager_getCategory(struct messagemanager_t* dispatcher, uint32_t category)
{
	if (!dispatcher)
		return 0;

	WriteLog(LL_Debug, "[+] got dispatcher %p category: %d", dispatcher, category);

	if (category >= RPCCAT_COUNT)
		return 0;

	struct messagecategory_t* rpccategory = 0;

	spin_lock(&dispatcher->lock);
	for (uint32_t i = 0; i < RPCDISPATCHER_MAX_CATEGORIES; ++i)
	{
		struct messagecategory_t* cat = dispatcher->categories[i];

		if (!cat)
			continue;

		if (cat->category == category)
		{
			rpccategory = dispatcher->categories[i];
			break;
		}
	}
	spin_unlock(&dispatcher->lock);

	return rpccategory;
}

int32_t messagemanager_registerCallback(struct messagemanager_t* dispatcher, uint32_t callbackCategory, uint32_t callbackType, void* callback)
{
	// Verify that the dispatcher and listener are valid
	if (!dispatcher || !callback)
		return 0;

	// Verify the listener category
	if (callbackCategory >= RPCCAT_COUNT)
		return 0;

	struct messagecategory_t* category = messagemanager_getCategory(dispatcher, callbackCategory);
	if (!category)
	{
		// Get a free listener index
		int32_t freeIndex = messagemanager_findFreeCategoryIndex(dispatcher);
		if (freeIndex == -1)
			return 0;

		// Allocate a new category
		category = (struct messagecategory_t*)kmalloc(sizeof(struct messagecategory_t));
		if (!category)
			return 0;

		// Initialize the category
		rpccategory_init(category, callbackCategory);

		// Set the category in our list
		dispatcher->categories[freeIndex] = category;
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

void messagemanager_sendMessage(struct messagemanager_t* dispatcher, struct allocation_t* ref)
{
	if (!dispatcher || !ref)
		return;

	struct message_t* message = __get(ref);
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

		if (message->header.payloadLength > 0 && message->payload != NULL && message->header.error == 0)
			kwrite(message->socket, message->payload, message->header.payloadLength);

		goto cleanup;
	}

	WriteLog(LL_Debug, "[+] sendMessage dispatcher: %p ref: %p message: %p", dispatcher, ref, message);

	if (message->header.category >= RPCCAT_COUNT)
	{
		WriteLog(LL_Error, "[-] invalid message category: %d max: %d", message->header.category, RPCCAT_COUNT);
		goto cleanup;
	}

	WriteLog(LL_Debug, "[+] getting dispatcher category");

	struct messagecategory_t* category = messagemanager_getCategory(dispatcher, message->header.category);
	if (!category)
	{
		WriteLog(LL_Debug, "[-] could not get dispatcher category");
		goto cleanup;
	}

	// Iterate through all of the callbacks
	WriteLog(LL_Debug, "[+] iterating callbacks");
	for (uint32_t l_CallbackIndex = 0; l_CallbackIndex < RPCCATEGORY_MAX_CALLBACKS; ++l_CallbackIndex)
	{
		// Get the category callback structure
		struct messagecategory_callback_t* l_Callback = category->callbacks[l_CallbackIndex];

		// Check to see if this callback is used at all
		if (!l_Callback)
			continue;

		// Check the type of the message
		if (l_Callback->type != message->header.type)
			continue;

		// Call the callback with the provided message
		WriteLog(LL_Debug, "[+] calling callback %p(%p)", l_Callback->callback, ref);
		l_Callback->callback(ref);
	}

cleanup:
	__dec(ref);
}

void messagemanager_sendSuccessMessage(struct messagemanager_t* dispatcher, struct allocation_t* ref)
{
	if (!dispatcher || !ref)
		return;

	struct message_t* message = __get(ref);
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
	message->header.error = 0;

	WriteLog(LL_Debug, "sending success network response back to PC");

	// Save the payload length
	uint16_t payloadLength = message->header.payloadLength;

	// Set set the payload length to 0 because we are not sending a payload
	message->header.payloadLength = 0;

	// Send response back to the PC
	kwrite(message->socket, &message->header, sizeof(message->header));

	// Set the payload length back so memory cleanup will happen normally
	message->header.payloadLength = payloadLength;

cleanup:
	message->header.request = request;
	__dec(ref);
}

void messagemanager_sendErrorMessage(struct messagemanager_t* dispatcher, struct allocation_t* ref, int32_t error)
{
	if (!dispatcher || !ref)
		return;

	struct message_t* message = __get(ref);
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
		message->header.error = 0;

		WriteLog(LL_Debug, "sending success network response back to PC");

		// Save the payload length
		uint16_t payloadLength = message->header.payloadLength;

		// Set set the payload length to 0 because we are not sending a payload
		message->header.payloadLength = 0;

		// The error should always be < 0
		message->header.error = error < 0 ? error : -error;

		// Send response back to the PC
		kwrite(message->socket, &message->header, sizeof(message->header));

		// Set the payload length back so memory cleanup will happen normally
		message->header.payloadLength = payloadLength;
	}

	message->header.request = request;

	__dec(ref);
}