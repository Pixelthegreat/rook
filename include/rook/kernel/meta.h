#ifndef ROOK_KERNEL_META_H
#define ROOK_KERNEL_META_H

#include <rook/core.h>
#include <rook/meta.h>

/*
 * Get next metadata entry for type
 *
 * 'entry' can be NULL, indicating to return the first
 * entry of the desired type.
 */
extern rk_meta_t *rk_meta_next(rk_meta_t *entry, rk_meta_type_t type);

#endif /* ROOK_KERNEL_META_H */
