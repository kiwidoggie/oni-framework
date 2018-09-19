#pragma once
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>

#include <oni/utils/types.h>

//// Reference counter structure
//struct allocation_t {
//	// Item that we are holding a reference to
//	void* data;
//
//	// size of the allocation (needed for kmem_free)
//	uint64_t size;
//
//	// Current reference count
//	volatile int64_t count;
//
//	// Lock
//	struct mtx lock;
//};
//
///*
//__malloc
//
//This function will create a new allocation_t object with specified size
//This returned allocation will be ref counted for automatic collection when reference count hits zero
//
//Returns pointer to allocation_t object or NULL if there was an error
//*/
//struct allocation_t* __malloc(uint64_t size);
//
///*
//__free
//
//Frees the allocated memory, and the allocation itself
//
//This function should never be called directly by a user
//*/
//void __free(struct allocation_t* allocation);
//
///*
//__get
//
//Gets the allocation of the requested size specified during __malloc and increases the reference count
//
//Every time this is called, there must be a __dec on function "leave"
//*/
//void* __get(struct allocation_t* allocation);
//
///*
//__inc
//
//Internal function for incrementing the reference count
//*/
//void __inc(struct allocation_t* allocation);
//
///*
//__dec
//
//Function for decreasing the reference count, or if reference count == 0 freeing the memory and allocation_t object
//*/
//void __dec(struct allocation_t* allocation);

/*
kmalloc

malloc(3) wrapper to decrease arguments
*/
void* kmalloc(size_t size);

/*
kfree

Frees the memory at `address` with size `size`
*/
void kfree(void* address, size_t size);

void* kcalloc(size_t n, size_t size);

void* krealloc(void* address, size_t size);