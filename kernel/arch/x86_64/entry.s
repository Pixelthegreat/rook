/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
	.globl _start
	.globl arch_enable_fpu

	.extern arch_main

.section .bss
.set stack_size, 0x20000
.align 16
stack: .skip stack_size

.section .text
/* entry point */
_start:
	leaq stack, %rsp
	addq $stack_size, %rsp
	movq %rsp, %rbp

	call arch_enable_fpu
	call arch_main

/* enable floating point unit and sse extensions */
arch_enable_fpu:
	pushq %rbp
	movq %rsp, %rbp

	clts
	movq %cr0, %rax
	andw $0xfffd, %ax
	orw $0x10, %ax
	movq %rax, %cr0
	fninit

	movq %cr0, %rax
	andw $0xfffb, %ax
	orw $0x2, %ax
	movq %rax, %cr0

	movq %cr4, %rdx
	orq $0x600, %rdx
	movq %rdx, %cr4

	pushq $0x1f80
	ldmxcsr (%rsp)
	addq $8, %rsp

	popq %rbp
	retq
