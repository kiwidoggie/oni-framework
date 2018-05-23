
#include <oni/messaging/messagecategory.h>
#include <oni/messaging/message.h>

#include <oni/utils/sys_wrappers.h>
#include <oni/utils/memory/allocator.h>

void rpccategory_init(struct messagecategory_t* dispatcherCategory, uint8_t category)
{
	if (!dispatcherCategory)
		return;

	if (category >= RPCCAT_MAX)
		return;

	dispatcherCategory->category = category;

	kmemset(dispatcherCategory->callbacks, 0, sizeof(dispatcherCategory->callbacks));
}

int32_t rpccategory_findFreeCallbackIndex(struct messagecategory_t* category)
{
	if (!category)
		return -1;

	for (uint32_t i = 0; i < RPCCATEGORY_MAX_CALLBACKS; ++i)
	{
		if (!category->callbacks[i])
			return i;
	}

	return -1;
}

void rpccategory_sendMessage(struct messagecategory_t* category, struct allocation_t* msg)
{
	if (!category || !msg)
		return;

	struct message_t* message = __get(msg);
	if (!message)
		return;

	if (message->header.category != category->category)
		goto cleanup;

	for (uint32_t i = 0; i < RPCCATEGORY_MAX_CALLBACKS; ++i)
	{
		struct messagecategory_callback_t* callback = category->callbacks[i];
		if (!callback)
			continue;

		if (callback->type != message->header.error_type)
			continue;

		if (!callback->callback)
			continue;

		callback->callback(msg);
	}

cleanup:
	__dec(msg);
}