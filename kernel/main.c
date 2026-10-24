/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/mmu.h>
#include <rook/kernel/heap.h>
#include <rook/kernel/meta.h>
#include <rook/kernel/module.h>
#include <rook/kernel/console.h>
#include <rook/kernel/acpi.h>
#include <rook/kernel/cpu.h>

extern void main(void) {

	rk_boot_init();
	rk_mmu_init();
	rk_heap_init();
	rk_module_init_builtins(0, 3);
	rk_console_init();
	rk_acpi_init();
	rk_cpu_init_all();
	rk_module_init_builtins(3, 100);
}
