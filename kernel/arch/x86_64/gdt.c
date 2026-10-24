/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <rook/core.h>
#include <rook/kernel/console.h>
#include <rook/kernel/arch/x86_64/gdt.h>

rk_gdt_entry_t rk_gdt[RK_GDT_ENTRY_COUNT];
rk_gdtr_t rk_gdtr;
rk_tss_t rk_tss;

/* load gdt */
extern void rk_gdt_load(rk_tss_t *tss) {

	if (!tss) tss = &rk_tss;

	rk_gdt[RK_GDT_ENTRY_KERNEL_CODE].limit_low = 0xffff;
	rk_gdt[RK_GDT_ENTRY_KERNEL_CODE].base_low = 0x0000;
	rk_gdt[RK_GDT_ENTRY_KERNEL_CODE].base_mid = 0x00;
	rk_gdt[RK_GDT_ENTRY_KERNEL_CODE].access = RK_GDT_ACCESS_P | RK_GDT_ACCESS_S | RK_GDT_ACCESS_E;
	rk_gdt[RK_GDT_ENTRY_KERNEL_CODE].flags = RK_GDT_FLAG_G | RK_GDT_FLAG_L | 0xf;
	rk_gdt[RK_GDT_ENTRY_KERNEL_CODE].base_high = 0x00;

	rk_gdt[RK_GDT_ENTRY_KERNEL_DATA].limit_low = 0xffff;
	rk_gdt[RK_GDT_ENTRY_KERNEL_DATA].base_low = 0x0000;
	rk_gdt[RK_GDT_ENTRY_KERNEL_DATA].base_mid = 0x00;
	rk_gdt[RK_GDT_ENTRY_KERNEL_DATA].access = RK_GDT_ACCESS_P | RK_GDT_ACCESS_S | RK_GDT_ACCESS_RW;
	rk_gdt[RK_GDT_ENTRY_KERNEL_DATA].flags = RK_GDT_FLAG_G | RK_GDT_FLAG_L | 0xf;
	rk_gdt[RK_GDT_ENTRY_KERNEL_DATA].base_high = 0x00;

	rk_gdt[RK_GDT_ENTRY_USER_CODE].limit_low = 0xffff;
	rk_gdt[RK_GDT_ENTRY_USER_CODE].base_low = 0x0000;
	rk_gdt[RK_GDT_ENTRY_USER_CODE].base_mid = 0x00;
	rk_gdt[RK_GDT_ENTRY_USER_CODE].access = RK_GDT_ACCESS_P | RK_GDT_ACCESS_S | RK_GDT_ACCESS_E | RK_GDT_ACCESS_DPL(3);
	rk_gdt[RK_GDT_ENTRY_USER_CODE].flags = RK_GDT_FLAG_G | RK_GDT_FLAG_L | 0xf;
	rk_gdt[RK_GDT_ENTRY_USER_CODE].base_high = 0x00;

	rk_gdt[RK_GDT_ENTRY_USER_DATA].limit_low = 0xffff;
	rk_gdt[RK_GDT_ENTRY_USER_DATA].base_low = 0x0000;
	rk_gdt[RK_GDT_ENTRY_USER_DATA].base_mid = 0x00;
	rk_gdt[RK_GDT_ENTRY_USER_DATA].access = RK_GDT_ACCESS_P | RK_GDT_ACCESS_S | RK_GDT_ACCESS_RW | RK_GDT_ACCESS_DPL(3);
	rk_gdt[RK_GDT_ENTRY_USER_DATA].flags = RK_GDT_FLAG_G | RK_GDT_FLAG_L | 0xf;
	rk_gdt[RK_GDT_ENTRY_USER_DATA].base_high = 0x00;

	rk_tss_entry_t *entry = (rk_tss_entry_t *)(rk_gdt + RK_GDT_ENTRY_TSS_LOW);

	entry->limit_low = sizeof(rk_tss_t);
	entry->base_low = (uint64_t)tss & 0xffff;
	entry->base_mid = ((uint64_t)tss >> 16) & 0xff;
	entry->access = RK_GDT_ACCESS_P | RK_GDT_ACCESS_TSS_AVAIL;
	entry->flags = RK_GDT_FLAG_G | RK_GDT_FLAG_L;
	entry->base_high = ((uint64_t)tss >> 24) & 0xff;
	entry->base_extended = (uint64_t)tss >> 32;

	rk_gdtr.size = sizeof(rk_gdt_entry_t) * RK_GDT_ENTRY_COUNT - 1;
	rk_gdtr.address = (uint64_t)rk_gdt;

	asm volatile(
		"lgdtq (rk_gdtr)\n"
		"movw $0x10, %ax\n"
		"movw %ax, %ds\n"
		"movw %ax, %es\n"
		"movw %ax, %fs\n"
		"movw %ax, %gs\n"
		"movw %ax, %ss\n"
		"movw $0x28, %ax\n"
		"ltr %ax\n"
		"pushq $0x8\n"
		"leaq (random_label), %rax\n"
		"pushq %rax\n"
		"retfq\n"
		"random_label:\n"
	);
}
