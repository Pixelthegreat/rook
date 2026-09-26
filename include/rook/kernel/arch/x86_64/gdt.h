#ifndef ROOK_KERNEL_ARCH_X86_64_GDT_H
#define ROOK_KERNEL_ARCH_X86_64_GDT_H

#include <rook/core.h>

/*
 * GDT register
 */
typedef struct rk_gdtr {
	uint16_t size;
	uint64_t address;
} __attribute__((packed)) rk_gdtr_t;

/*
 * Task State Segment
 */
typedef struct rk_tss {
	uint32_t rsvd1;
	uint64_t rsp[3];
	uint64_t rsvd2;
	uint64_t ist[7];
	uint8_t rsvd3[6];
	uint16_t iopb;
} __attribute__((packed)) rk_tss_t;

/*
 * GDT entries
 */
typedef enum rk_gdt_entry_index {
	RK_GDT_ENTRY_NULL = 0,

	RK_GDT_ENTRY_KERNEL_CODE,
	RK_GDT_ENTRY_KERNEL_DATA,
	RK_GDT_ENTRY_USER_CODE,
	RK_GDT_ENTRY_USER_DATA,
	RK_GDT_ENTRY_TSS_LOW,
	RK_GDT_ENTRY_TSS_HIGH,

	RK_GDT_ENTRY_COUNT,
} rk_gdt_entry_index_t;

#define RK_GDT_INDEX(i) ((i) << 3)

typedef struct rk_gdt_entry {
	uint16_t limit_low, base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t flags;
	uint8_t base_high;
} __attribute__((packed)) rk_gdt_entry_t;

typedef struct rk_tss_entry {
	uint16_t limit_low, base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t flags;
	uint8_t base_high;
	uint32_t base_extended;
	uint32_t rsvd;
} __attribute__((packed)) rk_tss_entry_t;

#define RK_GDT_ACCESS_A 0x1
#define RK_GDT_ACCESS_RW 0x2
#define RK_GDT_ACCESS_DC 0x4
#define RK_GDT_ACCESS_E 0x8
#define RK_GDT_ACCESS_S 0x10
#define RK_GDT_ACCESS_DPL(i) ((i) << 5)
#define RK_GDT_ACCESS_P 0x80

#define RK_GDT_ACCESS_LDT 0x2
#define RK_GDT_ACCESS_TSS_AVAIL 0x9
#define RK_GDT_ACCESS_TSS_BUSY 0xb

#define RK_GDT_FLAG_L 0x20
#define RK_GDT_FLAG_DB 0x40
#define RK_GDT_FLAG_G 0x80

extern rk_gdt_entry_t rk_gdt[RK_GDT_ENTRY_COUNT];
extern rk_gdtr_t rk_gdtr;
extern rk_tss_t rk_tss;

/*
 * Load GDT
 */
extern void gdt_load(rk_tss_t *tss);

#endif /* ROOK_KERNEL_ARCH_X86_64_GDT_H */
