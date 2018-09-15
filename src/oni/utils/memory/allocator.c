#include <oni/utils/memory/allocator.h>

#include <vm/vm.h>

#include <oni/utils/kdlsym.h>
#include <oni/utils/sys_wrappers.h>

#include <sys/malloc.h>

void* kmalloc(size_t size)
{
	/*void* M_TEMP = kdlsym(M_TEMP);
	void* (*malloc)(unsigned long size, struct malloc_type *type, int flags) = kdlsym(malloc);

	return malloc(size, M_TEMP, M_WAITOK | M_ZERO);*/

	vm_map_t map = (vm_map_t)(*(uint64_t *)(kdlsym(kernel_map)));
	vm_offset_t(*kmem_alloc)(vm_map_t map, vm_size_t size) = kdlsym(kmem_alloc);

	void* data = (void*)kmem_alloc(map, size);
	//if (data)
	//	kmlock(data, size);

	return data;
}

void kfree(void* address, size_t size)
{
	/*void (*free)(void *addr, struct malloc_type *type) = kdlsym(free);
	void* M_TEMP = kdlsym(M_TEMP);

	free(address, M_TEMP);*/

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

void* krealloc(void* address, size_t size)
{
	void* (*realloc)(void *addr, unsigned long size, struct malloc_type	*type, int flags) = kdlsym(realloc);
	void* M_TEMP = kdlsym(M_TEMP);

	return realloc(address, size, M_TEMP, M_WAITOK);
}