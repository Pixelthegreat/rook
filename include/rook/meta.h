#ifndef ROOK_META_H
#define ROOK_META_H

#include <rook/core.h>

/*
 * Kernel metadata records
 */
typedef enum rk_meta_type {
	RK_META_TYPE_BOOT_PROTOCOL = 0,
} rk_meta_type_t;

typedef struct rk_meta {
	uint32_t type, size;
} rk_meta_t;

#define RK_META __attribute__((used, section(".rk_meta"))) volatile

#endif /* ROOK_META_H */
