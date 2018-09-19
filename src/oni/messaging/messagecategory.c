
#include <oni/messaging/messagecategory.h>
#include <oni/messaging/message.h>

#include <oni/utils/kdlsym.h>
#include <oni/utils/memory/allocator.h>
#include <oni/utils/ref.h>

void rpccategory_init(struct messagecategory_t* dispatcherCategory, uint8_t category)
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	if (!dispatcherCategory)
		return;

	if (category >= RPCCAT_MAX)
		return;

	dispatcherCategory->category = category;

	memset(dispatcherCategory->callbacks, 0, sizeof(dispatcherCategory->callbacks));
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

void rpccategory_sendMessage(struct messagecategory_t* category, struct ref_t* msg)
{
	if (!category || !msg)
		return;

	struct message_header_t* message = ref_getDataAndAcquire(msg);
	if (!message)
		return;

	if (message->category != category->category)
		goto cleanup;

	for (uint32_t i = 0; i < RPCCATEGORY_MAX_CALLBACKS; ++i)
	{
		struct messagecategory_callback_t* callback = category->callbacks[i];
		if (!callback)
			continue;

		if (callback->type != message->error_type)
			continue;

		if (!callback->callback)
			continue;

		callback->callback(msg);
	}

cleanup:
	ref_release (msg);
}