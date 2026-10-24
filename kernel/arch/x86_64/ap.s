/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
	.extern _rk_ap_start

	.set status, 0x1200
	.set stack_pointer, 0x1208
	.set entry_point, 0x1210
	.set cr3_value, 0x1218

.section .data.ap
.code16
_ap_entry:
	cli
	cld
	leaw status, %di
	movw $1, (%di)
	ljmpw $0, $0x1040
.org 0x40
/* enter long mode */
_ap_1040:
	movl %cr4, %eax
	orl $0x20, %eax
	movl %eax, %cr4

	movl (cr3_value), %edx
	movl %edx, %cr3

	movl $0xc0000080, %ecx
	rdmsr
	orl $0x900, %eax
	wrmsr

	movl %cr0, %ebx
	movl $0x80010011, %ebx
	movl %ebx, %cr0

	cli
	lidtl 0x10c0 + (_ap_idtr - _ap_gdt)
	lgdtl 0x10c0 + (_ap_gdtr - _ap_gdt)
	ljmpl $8, $0x1100
/* global descriptor table */
.org 0xc0
_ap_gdt:
	.quad 0
	.quad 0
	.quad 0
_ap_gdtr:
	.word _ap_gdtr - _ap_gdt - 1
	.quad 0x10c0
/* interrupt descriptor table */
_ap_idtr:
	.word 0
	.long 0
/* 64-bit entry */
.org 0x100
.code64
_ap_1100:
	movq %cr3, %rax
	movq %rax, %cr3
	movq (stack_pointer), %rsp
	movq %rsp, %rbp
	movw $16, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	pushq $0
	pushq $8
	movq (entry_point), %rax
	pushq %rax
	retfq

.org 0x200
_ap_data:
	.quad 0 /* reset status */
	.quad 0 /* 64-bit stack pointer */
	.quad 0 /* 64-bit entry point */
	.quad 0 /* 64-bit cr3 value */
