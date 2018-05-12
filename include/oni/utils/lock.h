#pragma once
#define LOCK_PROFILING
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>


struct lock_t
{
	struct mtx mutex;
};

void spin_init(struct lock_t* lock);
void spin_lock(struct lock_t *lock);
void spin_unlock(struct lock_t *lock);