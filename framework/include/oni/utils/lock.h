



#pragma once
// Credits: https://github.com/cyfdecyf/spinlock/blob/54943d3b16c15701ddaccffee0ac3a124160cd4a/spinlock-cmpxchg.h

struct lock_t {
	volatile long lock;
};

#define SPINLOCK_INITIALIZER { 0 }

#ifdef _WIN32
#include <intrin.h>
#define SPINLOCK_ATTR
SPINLOCK_ATTR void spin_lock(struct lock_t *lock);
SPINLOCK_ATTR void spin_unlock(struct lock_t *s);
#else
#define SPINLOCK_ATTR static __inline __attribute__((always_inline, no_instrument_function))
#define cpu_relax() __asm__ volatile("pause\n": : :"memory")

SPINLOCK_ATTR char __testandset(struct lock_t *p)
{
	char readval = 0;

	__asm__ volatile (
		"lock; cmpxchgb %b2, %0"
		: "+m" (p->lock), "+a" (readval)
		: "r" (1)
		: "cc");
	return readval;
}

SPINLOCK_ATTR void spin_lock(struct lock_t *lock)
{
	while (__testandset(lock)) {
		/* Should wait until lock is free before another try.
		* cmpxchg is write to cache, competing write for a sinlge cache line
		* would generate large amount of cache traffic. That's why this
		* implementation is not scalable compared to xchg based one. Otherwise,
		* they should have similar performance. */
		cpu_relax();
	}
}

SPINLOCK_ATTR void spin_unlock(struct lock_t *s)
{
	s->lock = 0;
}
#endif

