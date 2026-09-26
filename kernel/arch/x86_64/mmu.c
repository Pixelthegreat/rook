#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/lock.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/console.h>
#include <rook/kernel/mmu.h>

extern uint64_t _rk_start;
extern uint64_t _rk_end;

#define PAGE_SIZE 0x1000
#define PAGE_SHIFT 12
#define PAGE_ENTRIES (PAGE_SIZE >> 3)

#define PAGE_P 0x1
#define PAGE_RW 0x2
#define PAGE_US 0x4
#define PAGE_PWT 0x8
#define PAGE_PCD 0x10
#define PAGE_A 0x20
#define PAGE_D 0x40
#define PAGE_PAT 0x80
#define PAGE_G 0x100
#define PAGE_XD 0x8000000000000000
#define PAGE_ADDR(p) ((p) & 0xFFFFFFFFFF000)

#define ENTRY_TO_ADDR(e, l, b) ((((e) << ((l) * 9 + 12)) + (b)) | 0xFFFF000000000000)

#define PAGE_ADDR_OFFSET(p) (((p) >= 0x800000000)? 0xFFFF000000000000: 0)
#define PAGE_TO_ADDR(p) (((p) << 12) | PAGE_ADDR_OFFSET(p))

uint64_t rk_mmu_page_size = PAGE_SIZE;
uint64_t rk_mmu_breakpoint = 0xFFFFFFFF00000000;

/* physical memory range / usage maps */
#define RANGE_SIZE 0x100000 /* 4G in frames */
#define RANGE_SHIFT 20
#define RANGE_MASK 0xfffff
#define MAX_RANGES 0x100

static struct range {
	size_t used_count;
	uint8_t frames[RANGE_SIZE];
} base; /* base 4G range */

static struct range *ranges[MAX_RANGES] = {&base,};
static bool needed[MAX_RANGES];

/* spare page table space */
#define MAX_SPARE_TABLES 128
static __attribute__((aligned(PAGE_SIZE), section(".bigbss")))
	uint64_t spare_tables[MAX_SPARE_TABLES * PAGE_ENTRIES];

static uint64_t spare_table_breakpoint = 0;
static uint64_t spare_table_base = 0;

#define NEXT_SPARE_TABLE ((uint64_t *)(spare_tables + (spare_table_breakpoint++) * PAGE_ENTRIES))

static void *clone_area = NULL;

/* page table wrap area */
#define PAGE_WRAP_ADDR_L4 0xFFFF800000000000 /* 256 << 39 */
#define PAGE_WRAP_ADDR_L3 0xFFFF804000000000 /* L4 + (256 << 30) */
#define PAGE_WRAP_ADDR_L2 0xFFFF804020000000 /* L3 + (256 << 21) */
#define PAGE_WRAP_ADDR_L1 0xFFFF804020100000 /* L2 + (256 << 12) */

#define PAGE_WRAP_INDEX 256

static uint64_t *page_wrap[4] = {
	(uint64_t *)PAGE_WRAP_ADDR_L1,
	(uint64_t *)PAGE_WRAP_ADDR_L2,
	(uint64_t *)PAGE_WRAP_ADDR_L3,
	(uint64_t *)PAGE_WRAP_ADDR_L4,
};
static rk_spinlock_t frame_lock;

static uint64_t main_cr3;

static bool init_mmu = false;

/* copy page table */
#define RANGES_OVERLAP(s1, e1, s2, e2) ((s1) <= (e2) && (s2) <= (e1))

static void copy_table(
		uint64_t base,
		uint64_t phys,
		uint64_t *new_table,
		uint64_t level,
		uint64_t level_base
) {
	uint64_t kernel_start = (uint64_t)&_rk_start & ~0xfff;
	uint64_t kernel_end = RK_ALIGN((uint64_t)&_rk_end, PAGE_SIZE);

	/* copy entries */
	uint64_t *old_table = (uint64_t *)(base + phys);
	for (size_t i = 0; i < PAGE_ENTRIES; i++) {

		uint64_t new_level_base = ENTRY_TO_ADDR(i, level, level_base);
		uint64_t start = new_level_base;
		new_level_base = PAGE_ADDR(new_level_base);
		uint64_t end = ENTRY_TO_ADDR(i+1, level, level_base);

		if (!old_table[i]) continue;

		if ((level == 3 && i == PAGE_ENTRIES-1) ||
		    RANGES_OVERLAP(start, end, kernel_start, kernel_end)) {

			if (level) {

				uint64_t *table = NEXT_SPARE_TABLE;
				uint64_t new_table_base =
					(uint64_t)table - (uint64_t)spare_tables +
					spare_table_base;

				copy_table(
					base,
					PAGE_ADDR(old_table[i]),
					table,
					level-1,
					new_level_base
				);
				new_table[i] = new_table_base | (old_table[i] & 0xfff);
			}
			else new_table[i] = old_table[i];
		}
	}
}

/* initialize mmu system */
extern void rk_mmu_init(void) {

	rk_spinlock_init(&frame_lock);

	asm volatile("movq %%cr3, %0": "=r"(main_cr3));

	/* process memory map */
	base.used_count = RANGE_SIZE;
	memset(&base.frames, 1, sizeof(base.frames));

	for (size_t i = 0; i < rk_boot_info.memory_map.count; i++) {

		rk_boot_memory_map_t *entry = rk_boot_info.memory_map.entries+i;

		if (entry->type != RK_BOOT_MEMORY_MAP_FREE)
			continue;

		uint64_t fstart = RK_ALIGN(entry->start, PAGE_SIZE) >> PAGE_SHIFT;
		uint64_t fend = entry->end >> PAGE_SHIFT;

		for (uint64_t j = fstart; j < fend; j++) {

			uint64_t range = j >> RANGE_SHIFT;

			if (range >= MAX_RANGES) break;
			else if (!ranges[range]) {

				needed[range] = true;
				break;
			}
			rk_mmu_frame_decref(j);
		}
	}

	/* copy page tables */
	uint64_t *new_table = NEXT_SPARE_TABLE;

	uint64_t n_table = (uint64_t)new_table;
	uint64_t n_page = (n_table >> 12) & 0x1ff;
	uint64_t n_pt1 = (n_table >> 21) & 0x1ff;
	uint64_t n_pt2 = (n_table >> 30) & 0x1ff;
	uint64_t n_pt3 = (n_table >> 39) & 0x1ff;

	uint64_t *pt4 = (uint64_t *)(rk_boot_info.hhdm.offset + main_cr3);
	uint64_t *pt3 = (uint64_t *)(PAGE_ADDR(pt4[n_pt3]) + rk_boot_info.hhdm.offset);
	uint64_t *pt2 = (uint64_t *)(PAGE_ADDR(pt3[n_pt2]) + rk_boot_info.hhdm.offset);
	uint64_t *pt1 = (uint64_t *)(PAGE_ADDR(pt2[n_pt1]) + rk_boot_info.hhdm.offset);

	uint64_t address = PAGE_ADDR(pt1[n_page]);
	spare_table_base = address;

	copy_table(rk_boot_info.hhdm.offset, main_cr3, new_table, 3, 0);

	new_table[PAGE_WRAP_INDEX] = address | PAGE_P | PAGE_RW;

	/* load cr3 */
	asm volatile("mov %0, %%cr3": : "r"(address));
	main_cr3 = address;

	rk_log_debug("Blink twice if you see this");

	/* mark the remainder of frames as used */
	for (size_t i = 0; i < MAX_RANGES; i++) {

		if (!needed[i]) continue;

		ranges[i] = rk_mmu_map(NULL, NULL, RANGE_SIZE, 0);
		memset(ranges[i]->frames, 1, RANGE_SIZE);
	}

	for (size_t i = 0; i < rk_boot_info.memory_map.count; i++) {

		rk_boot_memory_map_t *entry = rk_boot_info.memory_map.entries+i;
		if (entry->type != RK_BOOT_MEMORY_MAP_FREE)
			continue;

		uint64_t f_start = RK_ALIGN(entry->start, PAGE_SIZE) >> PAGE_SHIFT;
		uint64_t f_end = entry->end >> PAGE_SHIFT;

		for (uint64_t j = f_start; j < f_end; j++)
			rk_mmu_frame_decref(j);
	}

	/* remap physical memory ranges */
	if (rk_boot_info.framebuffer.virtual)
		rk_boot_info.framebuffer.virtual = rk_mmu_map(
				NULL,
				rk_boot_info.framebuffer.physical,
				rk_boot_info.framebuffer.size,
				0
		);

	init_mmu = true;
	rk_log_info("Initialized MMU");
}

/* increase reference count for page frame */
extern void rk_mmu_frame_incref(uint64_t id) {

	rk_spinlock_acquire(&frame_lock);

	uint64_t range = id >> RANGE_SHIFT;
	uint64_t frame = id & RANGE_MASK;

	if (range >= MAX_RANGES ||
	    !ranges[range] || ranges[range]->frames[frame] == 255) {

		rk_spinlock_release(&frame_lock);
		return;
	}
	if (!ranges[range]->frames[frame])
		ranges[range]->used_count++;
	ranges[range]->frames[frame]++;

	rk_spinlock_release(&frame_lock);
}

/* decrease reference count for page frame */
extern void rk_mmu_frame_decref(uint64_t id) {

	rk_spinlock_acquire(&frame_lock);

	uint64_t range = id >> RANGE_SHIFT;
	uint64_t frame = id & RANGE_MASK;

	if (range >= MAX_RANGES ||
	    !ranges[range] || !ranges[range]->frames[frame]) {

		rk_spinlock_release(&frame_lock);
		return;
	}
	ranges[range]->frames[frame]--;
	if (!ranges[range]->frames[frame])
		ranges[range]->used_count--;

	rk_spinlock_release(&frame_lock);
}

/* allocate page frame */
extern uint64_t rk_mmu_frame_allocate(void) {

	rk_spinlock_acquire(&frame_lock);

	uint64_t range = 0, frame;
	for (; range < MAX_RANGES; range++) {

		if (!ranges[range] || ranges[range]->used_count >= RANGE_SIZE)
			continue;
		for (frame = 0; frame < RANGE_SIZE; frame++) {
			if (!ranges[range]->frames[frame]) {

				ranges[range]->used_count++;
				ranges[range]->frames[frame]++;

				rk_spinlock_release(&frame_lock);
				return (range << RANGE_SHIFT) + frame;
			}
		}
	}
	rk_spinlock_release(&frame_lock);
	return 0;
}

/* invalidate a page */
static void invalidate(uint64_t page) {

	uint64_t address = PAGE_TO_ADDR(page);
	asm volatile("invlpg (%0)": : "r"(address));
}

static void invalidate_global(uint64_t page) {

	invalidate(page);

	/* TODO: TLB shootdowns */
}

/* map page */
extern bool rk_mmu_page_map(
		uint64_t page,
		uint64_t frame,
		uint64_t flags
) {
	page &= 0xfffffffff;

	uint64_t page_flags = PAGE_P | PAGE_RW;

	if ((page << PAGE_SHIFT) >= 0 &&
	    (page << PAGE_SHIFT) < 0x800000000000)
		page_flags |= PAGE_US;

	uint64_t table_flags = page_flags;

	if (flags & RK_MMU_READ_ONLY)
		page_flags &= ~(PAGE_RW);

	/* allocate necessary tables */
	for (uint64_t i = 0; i < 3; i++) {

		uint64_t index = page >> ((3-i) * 9);
		uint64_t ipage = ((uint64_t)page_wrap[i+1] >> PAGE_SHIFT) + index;

		uint8_t *address = (uint8_t *)(ipage << PAGE_SHIFT);
		if (!page_wrap[i][index]) {

			uint64_t ftable = rk_mmu_frame_allocate();
			if (!ftable) return false;

			page_wrap[i][index] = PAGE_ADDR(ftable << PAGE_SHIFT) | table_flags;
			invalidate_global(ipage);

			memset(address, 0, PAGE_SIZE);
		}
	}

	/* map page */
	if (page_wrap[3][page])
		rk_mmu_frame_decref(PAGE_ADDR(page_wrap[3][page]) >> PAGE_SHIFT);

	page_wrap[3][page] = PAGE_ADDR(frame << PAGE_SHIFT) | page_flags;
	invalidate_global(page);

	return true;
}

/* unmap a page */
extern uint64_t rk_mmu_page_unmap(uint64_t page) {

	page &= 0xfffffffff;
	for (uint64_t i = 0; i < 3; i++) {

		uint64_t index = page >> ((3-i) * 9);
		if (!page_wrap[i][index])
			return 0;
	}
	uint64_t frame = page_wrap[3][page] >> PAGE_SHIFT;

	page_wrap[3][page] = 0;
	invalidate_global(page);

	return frame;
}

/* get frame for page */
extern uint64_t rk_mmu_page_get_frame(uint64_t page) {

	page &= 0xfffffffff;
	for (uint64_t i = 0; i < 3; i++) {

		uint64_t index = page >> ((3-i) * 9);
		if (!page_wrap[i][index])
			return 0;
	}
	return page_wrap[3][page] >> PAGE_SHIFT;
}

/* map a range of memory */
extern void *rk_mmu_map(
		void *virtual,
		void *physical,
		size_t size,
		uint64_t flags
) {
	uint64_t u_virtual = (uint64_t)virtual;
	uint64_t u_physical = (uint64_t)physical;

	if (!u_virtual) u_virtual = rk_mmu_breakpoint;

	if ((u_virtual & (PAGE_SIZE-1)) != 0 ||
	    (u_physical & (PAGE_SIZE-1)) != 0)
		return NULL;

	size = RK_ALIGN(size, PAGE_SIZE);

	uint64_t start = u_virtual >> PAGE_SHIFT;
	uint64_t page_count = size >> PAGE_SHIFT;

	/* map pages */
	for (uint64_t i = 0; i < page_count; i++) {

		uint64_t frame;
		if (u_physical) {

			frame = (u_physical >> PAGE_SHIFT) + i;
			rk_mmu_frame_incref(frame);
		}
		else {
			frame = rk_mmu_frame_allocate();
			if (!frame) return NULL;
		}
		uint64_t page = (start + i) & 0xFFFFFFFFF;
		if (!rk_mmu_page_map(page, frame, flags)) return NULL;
	}
	if (u_virtual == rk_mmu_breakpoint)
		rk_mmu_breakpoint += size;
	return (void *)u_virtual;
}

/* unmap a range of memory */
extern void rk_mmu_unmap(void *virtual, size_t size) {

	uint64_t u_virtual = (uint64_t)virtual;
	if ((u_virtual & (PAGE_SIZE-1)) != 0)
		return;

	size = RK_ALIGN(size, PAGE_SIZE);

	uint64_t start = u_virtual >> PAGE_SHIFT;
	uint64_t page_count = size >> PAGE_SHIFT;

	/* unmap pages */
	for (uint64_t i = 0; i < page_count; i++) {

		uint64_t frame = rk_mmu_page_unmap(start + i);
		if (frame) rk_mmu_frame_decref(frame);
	}
}

/* check if region of memory is mapped */
extern bool rk_mmu_is_mapped(void *virtual, size_t size) {

	if (!init_mmu) return false;

	uint64_t start = (uint64_t)virtual >> PAGE_SHIFT;
	uint64_t end = RK_ALIGN((uint64_t)virtual + size, PAGE_SIZE) >> PAGE_SHIFT;

	for (uint64_t i = start; i < end; i++) {
		if (!rk_mmu_page_get_frame(i))
			return false;
	}
	return true;
}

/* invalidate memory region */
extern void rk_mmu_invalidate(void *virtual) {

	invalidate(PAGE_ADDR((uint64_t)virtual) >> PAGE_SHIFT);
}

/* get physical address for virtual address */
extern void *rk_mmu_get_map(void *virtual) {

	uint64_t page = PAGE_ADDR((uint64_t)virtual) >> PAGE_SHIFT;
	return (void *)(rk_mmu_page_get_frame(page) << PAGE_SHIFT);
}
