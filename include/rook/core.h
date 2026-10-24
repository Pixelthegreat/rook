/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ROOK_CORE_H
#define ROOK_CORE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define RK_ALIGN(x, sz) (((x) + ((sz)-1)) & ~((sz)-1))

#define RK_MIN(x, y) ((x) < (y)? (x): (y))
#define RK_MAX(x, y) ((x) > (y)? (x): (y))

#define RK_ABS(x) ((x) < 0? -(x): (x))

#endif /* ROOK_CORE_H */
