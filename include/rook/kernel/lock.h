#ifndef ROOK_KERNEL_LOCK_H
#define ROOK_KERNEL_LOCK_H

#include <rook/core.h>

typedef struct rk_spinlock {
	int v;
} rk_spinlock_t;

/*
 * Initialize lock
 */
extern void rk_spinlock_init(rk_spinlock_t *lock); //arch//

/*
 * Acquire lock
 */
extern void rk_spinlock_acquire(rk_spinlock_t *lock); //arch//

/*
 * Release lock
 */
extern void rk_spinlock_release(rk_spinlock_t *lock); //arch//

#endif /* ROOK_KERNEL_LOCK_H */
