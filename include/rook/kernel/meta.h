#ifndef ROOK_KERNEL_META_H
#define ROOK_KERNEL_META_H

#include <rook/core.h>

/*
 * Kernel metadata records
 */
typedef enum rk_meta_type {
	RK_META_TYPE_BOOT_PROTOCOL = 0,
	RK_META_TYPE_MODULE,
} rk_meta_type_t;

typedef struct rk_meta {
	uint32_t type, size;
} __attribute__((packed)) rk_meta_t;

#define RK_META(...) __attribute__((used, section(".rk_meta" __VA_OPT__(".") #__VA_ARGS__), aligned(1))) volatile

/*
 * Get next metadata entry for type
 *
 * 'entry' can be NULL, indicating to return the first
 * entry of the desired type.
 */
extern rk_meta_t *rk_meta_next(rk_meta_t *entry, rk_meta_type_t type);

#endif /* ROOK_KERNEL_META_H */
