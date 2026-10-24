/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ROOK_KERNEL_CPU_H
#define ROOK_KERNEL_CPU_H

#include <rook/core.h>
#include <rook/kernel/lock.h>
#include <rook/kernel/device.h>

#define RK_CPU_COMMAND_PING 0x0
#define RK_CPU_COMMAND_SHOOTDOWN 0x1
#define RK_CPU_COMMAND_READY 0xfe
#define RK_CPU_COMMAND_HALT 0xff

/*
 * CPU device
 */
typedef struct rk_cpu_device {
	rk_device_t base;

	size_t id;
	void *arch_data[4];
	bool present, active, ready;

	struct {
		rk_spinlock_t lock;
		uint64_t command, userdata;
		bool acknowledged;
	} signal;

	struct {
		size_t count;
	} lock; /* intra-thread lock */

	rk_spinlock_t global_lock;
} rk_cpu_device_t;

#define RK_CPU_DEVICE(p) ((rk_cpu_device_t *)(p))

extern rk_device_class_t rk_device_class_cpu;

/*
 * Initialize all CPUs
 */
extern void rk_cpu_init_all(void); //arch//

/*
 * Check if CPU in question is ourself
 */
extern bool rk_cpu_is_self(rk_device_t *device); //arch//

/*
 * Register CPU if not already registered
 */
extern rk_device_t *rk_cpu_register(size_t id);

/*
 * Get CPU for id
 */
extern rk_device_t *rk_cpu_get(size_t id);

/*
 * Get next active CPU
 */
extern rk_device_t *rk_cpu_get_next_active(rk_device_t *device);

/*
 * Send signal to CPU
 */
extern void rk_cpu_signal(
		rk_device_t *device,
		uint64_t command,
		uint64_t userdata
); //arch//

/*
 * Handle signal
 */
extern void rk_cpu_signaled(rk_device_t *device);

/*
 * Send signal to all CPUs
 */
extern void rk_cpu_signal_all(
		uint64_t command,
		uint64_t userdata
);

/*
 * Get ourself
 */
extern rk_device_t *rk_cpu_get_self(void); //arch//

/*
 * Lock and unlock cpu (intra-thread lock)
 */
extern void rk_cpu_lock(void); //arch//
extern void rk_cpu_unlock(void); //arch//

#endif /* ROOK_KERNEL_CPU_H */
