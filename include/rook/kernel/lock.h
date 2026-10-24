/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ROOK_KERNEL_LOCK_H
#define ROOK_KERNEL_LOCK_H

#include <rook/core.h>

typedef struct rk_spinlock {
	int v;
} rk_spinlock_t;

typedef rk_spinlock_t rk_resource_t;

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

/*
 * Acquire resource
 *
 * This is the same as 'rk_spinlock_acquire' except
 * it only attempts to lock once. This returns true
 * if said attempt was successful, and false otherwise.
 */
extern bool rk_resource_acquire(rk_resource_t *resource); //arch//

#define rk_resource_release rk_spinlock_release

#endif /* ROOK_KERNEL_LOCK_H */
