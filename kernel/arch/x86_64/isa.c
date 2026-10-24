/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <rook/core.h>
#include <rook/kernel/arch/x86_64/isa.h>

/* write byte to isa port */
extern void rk_isa_outb(uint16_t address, uint8_t value) {

	asm volatile("outb %%al, %%dx": : "a"(value), "d"(address));
}

/* read byte from isa port */
extern uint8_t rk_isa_inb(uint16_t address) {

	uint8_t value;
	asm volatile("inb %%dx, %%al": "=a"(value): "d"(address));
	return value;
}
