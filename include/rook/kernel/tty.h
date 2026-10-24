/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ROOK_KERNEL_TTY_H
#define ROOK_KERNEL_TTY_H

#include <rook/core.h>
#include <rook/kernel/lock.h>
#include <rook/kernel/device.h>

/*
 * TTY device
 */
typedef struct rk_tty_device {
	rk_device_t base;
	rk_spinlock_t read_lock;
	rk_spinlock_t write_lock;
	size_t (*read)(struct rk_tty_device *device, char *buffer, size_t count);
	size_t (*write)(struct rk_tty_device *device, const char *buffer, size_t count);
} rk_tty_device_t;

#define RK_TTY_DEVICE(p) ((rk_tty_device_t *)(p))

extern rk_device_class_t rk_device_class_tty;

/*
 * Read from TTY
 */
extern size_t rk_tty_device_read(rk_device_t *device, char *buffer, size_t count);

/*
 * Write to TTY
 */
extern size_t rk_tty_device_write(rk_device_t *device, const char *buffer, size_t count);

#endif /* ROOK_KERNEL_TTY_H */
