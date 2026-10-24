/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ROOK_KERNEL_HEAP_H
#define ROOK_KERNEL_HEAP_H

#include <rook/core.h>

/*
 * Initialize heap
 */
extern void rk_heap_init(void);

/*
 * Standard memory allocation functions
 */
extern void *malloc(size_t size);
extern void *realloc(void *pointer, size_t size);
extern void free(void *pointer);

#endif /* ROOK_KERNEL_HEAP_H */
