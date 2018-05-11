#include <oni/utils/lock.h>

#include <oni/utils/kdlsym.h>
#include <oni/utils/types.h>


void spin_init(struct lock_t* lock)
{
	void(*mtx_init)(struct mtx *m, const char *name, const char *type, int opts) = kdlsym(mtx_init);
	mtx_init(&lock->mutex, NULL, NULL, 0);
}

void spin_lock(struct lock_t *lock)
{
	void(*_mtx_lock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_lock_flags);
	_mtx_lock_flags(&lock->mutex, 0, NULL, 0);
}

void spin_unlock(struct lock_t *lock)
{
	void(*_mtx_unlock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_unlock_flags);
	_mtx_unlock_flags(&lock->mutex, 0, NULL, 0);
}