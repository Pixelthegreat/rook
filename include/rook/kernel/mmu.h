/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ROOK_KERNEL_MMU_H
#define ROOK_KERNEL_MMU_H

#include <rook/core.h>

#define RK_MMU_READ_ONLY 0x1

/*
 * System attributes
 */
extern uint64_t rk_mmu_page_size;
extern uint64_t rk_mmu_breakpoint;

/*
 * Initialize the MMU system
 */
extern void rk_mmu_init(void); //arch//

/*
 * Manage page frame allocation
 */
extern void rk_mmu_frame_incref(uint64_t id); //arch//
extern void rk_mmu_frame_decref(uint64_t id); //arch//
extern uint64_t rk_mmu_frame_allocate(void); //arch//

/*
 * Map page
 */
extern bool rk_mmu_page_map(
		uint64_t page,
		uint64_t frame,
		uint64_t flags
); //arch//

/*
 * Unmap page
 */
extern uint64_t rk_mmu_page_unmap(uint64_t page); //arch//

/*
 * Get frame for page
 */
extern uint64_t rk_mmu_page_get_frame(uint64_t page); //arch//

/*
 * Map a range of memory
 */
extern void *rk_mmu_map(
		void *virtual,
		void *physical,
		size_t size,
		uint64_t flags
); //arch//

/*
 * Unmap a range of memory
 */
extern void rk_mmu_unmap(void *virtual, size_t size); //arch//

/*
 * Determine if a region of memory is mapped
 */
extern bool rk_mmu_is_mapped(void *virtual, size_t size); //arch//

/*
 * Invalidate an address
 */
extern void rk_mmu_invalidate(void *virtual); //arch//

/*
 * Get physical address for virtual address
 */
extern void *rk_mmu_get_map(void *virtual); //arch//

#endif /* ROOK_KERNEL_MMU_H */
