#include <rook/core.h>
#include <rook/kernel/lock.h>

/* initialize lock */
extern void rk_spinlock_init(rk_spinlock_t *lock) {

	lock->v = 0;
}

/* acquire lock */
extern void rk_spinlock_acquire(rk_spinlock_t *lock) {

	int v = 1;
	while (v) asm volatile("xchgl %0, %1": "+m"(lock->v), "=a"(v));
}

/* release lock */
extern void rk_spinlock_release(rk_spinlock_t *lock) {

	lock->v = 0;
}
