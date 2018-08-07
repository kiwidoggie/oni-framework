#include <oni/utils/memory/allocator.h>

#include <vm/vm.h>

#include <oni/utils/kdlsym.h>
#include <oni/utils/sys_wrappers.h>

struct allocation_t* __malloc(uint64_t size)
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);
	void(*mtx_init)(struct mtx *m, const char *name, const char *type, int opts) = kdlsym(mtx_init);

	if (!size)
		return 0;

	struct allocation_t* allocation = (struct allocation_t*)kmalloc(sizeof(struct allocation_t));
	if (!allocation)
		return 0;

	// Added bonus of zeroing buffers
	memset(allocation, 0, sizeof(struct allocation_t));

	void* data = kmalloc(size);
	if (!data)
	{
		kfree(allocation, sizeof(struct allocation_t));
		allocation = 0;
		return 0;
	}

	// Added bonus of zeroing buffers
	memset(data, 0, size);

	allocation->data = data;
	allocation->size = size;
	allocation->count = 0;
	mtx_init(&allocation->lock, "allocmtx", NULL, 0);

	return allocation;
}

void __free(struct allocation_t* allocation)
{
	vm_map_t map = (vm_map_t)(*(uint64_t *)(kdlsym(kernel_map)));
	void(*kmem_free)(void* map, void* addr, size_t size) = kdlsym(kmem_free);
	void(*mtx_init)(struct mtx *m, const char *name, const char *type, int opts) = kdlsym(mtx_init);

	if (!allocation)
		return;

	if (allocation->data)
	{
		kmem_free(map, allocation->data, allocation->size);
		allocation->data = 0;
	}

	allocation->count = 0;
	mtx_init(&allocation->lock, "allocmtx", NULL, 0);
	allocation->size = 0;

	kmem_free(map, allocation, sizeof(struct allocation_t));
}

// On checking the result of __get, do NOT call __dec as the ref count is only incremented on success
void* __get(struct allocation_t* allocation)
{
	// Ensure the reference, item are valid
	if (!allocation || !allocation->data)
		return 0;

	// Increment the ref count
	__inc(allocation);

	// Return the referenced item
	return allocation->data;
}

// Increment reference, this shouldn't be called by the user directly
void __inc(struct allocation_t* allocation)
{
	void(*_mtx_lock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_lock_flags);
	void(*_mtx_unlock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_unlock_flags);

	if (!allocation)
		return;

	_mtx_lock_flags(&allocation->lock, 0, __FILE__, __LINE__);
	allocation->count++;
	_mtx_unlock_flags(&allocation->lock, 0, __FILE__, __LINE__);
}

// Decrement reference count, this SHOULD be called by the user directly
void __dec(struct allocation_t* allocation)
{
	void(*_mtx_lock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_lock_flags);
	void(*_mtx_unlock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_unlock_flags);

	if (!allocation)
		return;

	_mtx_lock_flags(&allocation->lock, 0, __FILE__, __LINE__);
	allocation->count--;
	_mtx_unlock_flags(&allocation->lock, 0, __FILE__, __LINE__);

	if (allocation->count <= 0)
		kfree(allocation, sizeof(*allocation));
}

void* kmalloc(size_t size)
{
	vm_map_t map = (vm_map_t)(*(uint64_t *)(kdlsym(kernel_map)));
	vm_offset_t(*kmem_alloc)(vm_map_t map, vm_size_t size) = kdlsym(kmem_alloc);

	void* data = (void*)kmem_alloc(map, size);
	if (data)
		kmlock(data, size);

	return data;
}

void kfree(void* address, size_t size)
{
	vm_map_t map = (vm_map_t)(*(uint64_t *)(kdlsym(kernel_map)));
	void(*kmem_free)(void* map, void* addr, size_t size) = kdlsym(kmem_free);

	kmem_free(map, address, size);
}

void* kcalloc(size_t n, size_t size)
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	size_t total = n * size;
	void *p = kmalloc(total);

	if (!p) return NULL;

	return memset(p, 0, total);
}