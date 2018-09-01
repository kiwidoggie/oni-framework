#include <oni/utils/ref.h>
#include <oni/utils/kdlsym.h>
#include <oni/utils/memory/allocator.h>

static void* ref_getInternal(struct ref_t* reference);

struct ref_t* ref_alloc(size_t size)
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	// Verify valid size
	if (size == 0)
		return NULL;

	// Allocate memory for our reference counter
	size_t allocationSize = sizeof(struct ref_t) + size;

	struct ref_t* reference = (struct ref_t*)kmalloc(allocationSize);
	if (!reference)
		return NULL;

	// Zero reference
	memset(reference, 0, allocationSize);

	reference->size = size;
	reference->count = 0;

	// Increment reference count
	ref_acquire(reference);

	return reference;
}

static void* ref_getInternal(struct ref_t* reference)
{
	if (!reference)
		return NULL;

	return &reference->data;
}

void ref_acquire(struct ref_t* reference)
{
	if (!reference)
		return;

	__sync_fetch_and_add(&reference->count, 1);
}

void ref_release(struct ref_t* reference)
{
	if (!reference)
		return;

	// If we subtract and the reference count is zero, free the reference and data
	if (__sync_sub_and_fetch(&reference->count, 1) == 0)
	{
		// Get the allocation size + size of the header
		uint64_t allocationSize = reference->size + sizeof(struct ref_t);

		// Free the entire reference header + data afterwards
		kfree(reference, allocationSize);
	}
}

void* ref_getData(struct ref_t* reference)
{
	if (!reference)
		return NULL;

	if (reference->count < 1)
		return NULL;

	return ref_getInternal(reference);
}

void* ref_getIncrement(struct ref_t* reference)
{
	void* data = ref_getData(reference);
	if (!data)
		return NULL;

	ref_acquire(reference);

	return data;
}
