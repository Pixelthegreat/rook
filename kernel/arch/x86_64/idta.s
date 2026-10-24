/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
.globl rk_isr0
.globl rk_isr1
.globl rk_isr2
.globl rk_isr3
.globl rk_isr4
.globl rk_isr5
.globl rk_isr6
.globl rk_isr7
.globl rk_isr8
.globl rk_isr9
.globl rk_isr10
.globl rk_isr11
.globl rk_isr12
.globl rk_isr13
.globl rk_isr14
.globl rk_isr15
.globl rk_isr16
.globl rk_isr17
.globl rk_isr18
.globl rk_isr19
.globl rk_isr20
.globl rk_isr21
.globl rk_isr22
.globl rk_isr23
.globl rk_isr24
.globl rk_isr25
.globl rk_isr26
.globl rk_isr27
.globl rk_isr28
.globl rk_isr29
.globl rk_isr30
.globl rk_isr31

.globl rk_irq48
.globl rk_irq49
.globl rk_irq50
.globl rk_irq51
.globl rk_irq52
.globl rk_irq53
.globl rk_irq54
.globl rk_irq55
.globl rk_irq56
.globl rk_irq57
.globl rk_irq58
.globl rk_irq59
.globl rk_irq60
.globl rk_irq61
.globl rk_irq62
.globl rk_irq63
.globl rk_irq64
.globl rk_irq65
.globl rk_irq66
.globl rk_irq67
.globl rk_irq68
.globl rk_irq69
.globl rk_irq70
.globl rk_irq71
.globl rk_irq72
.globl rk_irq73
.globl rk_irq74
.globl rk_irq75
.globl rk_irq76
.globl rk_irq77
.globl rk_irq78
.globl rk_irq79
.globl rk_irq80
.globl rk_irq81
.globl rk_irq82
.globl rk_irq83
.globl rk_irq84
.globl rk_irq85
.globl rk_irq86
.globl rk_irq87
.globl rk_irq88
.globl rk_irq89
.globl rk_irq90
.globl rk_irq91
.globl rk_irq92
.globl rk_irq93
.globl rk_irq94
.globl rk_irq95
.globl rk_irq96
.globl rk_irq97
.globl rk_irq98
.globl rk_irq99
.globl rk_irq100
.globl rk_irq101
.globl rk_irq102
.globl rk_irq103
.globl rk_irq104
.globl rk_irq105
.globl rk_irq106
.globl rk_irq107
.globl rk_irq108
.globl rk_irq109
.globl rk_irq110
.globl rk_irq111
.globl rk_irq112
.globl rk_irq113
.globl rk_irq114
.globl rk_irq115
.globl rk_irq116
.globl rk_irq117
.globl rk_irq118
.globl rk_irq119
.globl rk_irq120
.globl rk_irq121
.globl rk_irq122
.globl rk_irq123
.globl rk_irq124
.globl rk_irq125
.globl rk_irq126
.globl rk_irq127

.globl rk_irqsys

.globl rk_irqipi
.globl rk_irqtimer

.extern rk_idt_isr_handler
.extern rk_idt_irq_handler

.section .text

/* isr handlers */
rk_isr0:
	pushq $0
	pushq $0
	jmp rk_isr_common_stub

rk_isr1:
	pushq $0
	pushq $1
	jmp rk_isr_common_stub
rk_isr2:
	pushq $0
	pushq $2
	jmp rk_isr_common_stub

rk_isr3:
	pushq $0
	pushq $3
	jmp rk_isr_common_stub

rk_isr4:
	pushq $0
	pushq $4
	jmp rk_isr_common_stub

rk_isr5:
	pushq $0
	pushq $5
	jmp rk_isr_common_stub

rk_isr6:
	pushq $0
	pushq $6
	jmp rk_isr_common_stub

rk_isr7:
	pushq $0
	pushq $7
	jmp rk_isr_common_stub

rk_isr8:
	pushq $8
	jmp rk_isr_common_stub

rk_isr9:
	pushq $0
	pushq $9
	jmp rk_isr_common_stub

rk_isr10:
	pushq $10
	jmp rk_isr_common_stub

rk_isr11:
	pushq $11
	jmp rk_isr_common_stub

rk_isr12:
	pushq $12
	jmp rk_isr_common_stub

rk_isr13:
	pushq $13
	jmp rk_isr_common_stub

rk_isr14:
	pushq $14
	jmp rk_isr_common_stub

rk_isr15:
	pushq $0
	pushq $15
	jmp rk_isr_common_stub

rk_isr16:
	pushq $0
	pushq $16
	jmp rk_isr_common_stub

rk_isr17:
	pushq $17
	jmp rk_isr_common_stub

rk_isr18:
	pushq $0
	pushq $18
	jmp rk_isr_common_stub

rk_isr19:
	pushq $0
	pushq $19
	jmp rk_isr_common_stub

rk_isr20:
	pushq $0
	pushq $20
	jmp rk_isr_common_stub

rk_isr21:
	pushq $21
	jmp rk_isr_common_stub

rk_isr22:
	pushq $0
	pushq $22
	jmp rk_isr_common_stub

rk_isr23:
	pushq $0
	pushq $23
	jmp rk_isr_common_stub

rk_isr24:
	pushq $0
	pushq $24
	jmp rk_isr_common_stub

rk_isr25:
	pushq $0
	pushq $25
	jmp rk_isr_common_stub

rk_isr26:
	pushq $0
	pushq $26
	jmp rk_isr_common_stub

rk_isr27:
	pushq $0
	pushq $27
	jmp rk_isr_common_stub

rk_isr28:
	pushq $0
	pushq $28
	jmp rk_isr_common_stub

rk_isr29:
	pushq $0
	pushq $29
	jmp rk_isr_common_stub

rk_isr30:
	pushq $0
	pushq $30
	jmp rk_isr_common_stub

rk_isr31:
	pushq $0
	pushq $31
	jmp rk_isr_common_stub

/* irq handlers */
rk_irq48:
	pushq $0
	pushq $48
	jmp rk_irq_common_stub

rk_irq49:
	pushq $0
	pushq $49
	jmp rk_irq_common_stub

rk_irq50:
	pushq $0
	pushq $50
	jmp rk_irq_common_stub

rk_irq51:
	pushq $0
	pushq $51
	jmp rk_irq_common_stub

rk_irq52:
	pushq $0
	pushq $52
	jmp rk_irq_common_stub

rk_irq53:
	pushq $0
	pushq $53
	jmp rk_irq_common_stub

rk_irq54:
	pushq $0
	pushq $54
	jmp rk_irq_common_stub

rk_irq55:
	pushq $0
	pushq $55
	jmp rk_irq_common_stub

rk_irq56:
	pushq $0
	pushq $56
	jmp rk_irq_common_stub

rk_irq57:
	pushq $0
	pushq $57
	jmp rk_irq_common_stub

rk_irq58:
	pushq $0
	pushq $58
	jmp rk_irq_common_stub

rk_irq59:
	pushq $0
	pushq $59
	jmp rk_irq_common_stub

rk_irq60:
	pushq $0
	pushq $60
	jmp rk_irq_common_stub

rk_irq61:
	pushq $0
	pushq $61
	jmp rk_irq_common_stub

rk_irq62:
	pushq $0
	pushq $62
	jmp rk_irq_common_stub

rk_irq63:
	pushq $0
	pushq $63
	jmp rk_irq_common_stub

rk_irq64:
	pushq $64
	pushq $48
	jmp rk_irq_common_stub

rk_irq65:
	pushq $0
	pushq $65
	jmp rk_irq_common_stub

rk_irq66:
	pushq $0
	pushq $66
	jmp rk_irq_common_stub

rk_irq67:
	pushq $0
	pushq $67
	jmp rk_irq_common_stub

rk_irq68:
	pushq $0
	pushq $68
	jmp rk_irq_common_stub

rk_irq69:
	pushq $0
	pushq $69
	jmp rk_irq_common_stub

rk_irq70:
	pushq $0
	pushq $70
	jmp rk_irq_common_stub

rk_irq71:
	pushq $0
	pushq $71
	jmp rk_irq_common_stub

rk_irq72:
	pushq $0
	pushq $72
	jmp rk_irq_common_stub

rk_irq73:
	pushq $0
	pushq $73
	jmp rk_irq_common_stub

rk_irq74:
	pushq $0
	pushq $74
	jmp rk_irq_common_stub

rk_irq75:
	pushq $0
	pushq $75
	jmp rk_irq_common_stub

rk_irq76:
	pushq $0
	pushq $76
	jmp rk_irq_common_stub

rk_irq77:
	pushq $0
	pushq $77
	jmp rk_irq_common_stub

rk_irq78:
	pushq $0
	pushq $78
	jmp rk_irq_common_stub

rk_irq79:
	pushq $0
	pushq $79
	jmp rk_irq_common_stub

rk_irq80:
	pushq $0
	pushq $80
	jmp rk_irq_common_stub

rk_irq81:
	pushq $0
	pushq $81
	jmp rk_irq_common_stub

rk_irq82:
	pushq $0
	pushq $82
	jmp rk_irq_common_stub

rk_irq83:
	pushq $0
	pushq $83
	jmp rk_irq_common_stub

rk_irq84:
	pushq $0
	pushq $84
	jmp rk_irq_common_stub

rk_irq85:
	pushq $0
	pushq $85
	jmp rk_irq_common_stub

rk_irq86:
	pushq $0
	pushq $86
	jmp rk_irq_common_stub

rk_irq87:
	pushq $0
	pushq $87
	jmp rk_irq_common_stub

rk_irq88:
	pushq $0
	pushq $88
	jmp rk_irq_common_stub

rk_irq89:
	pushq $0
	pushq $89
	jmp rk_irq_common_stub

rk_irq90:
	pushq $0
	pushq $90
	jmp rk_irq_common_stub

rk_irq91:
	pushq $0
	pushq $91
	jmp rk_irq_common_stub

rk_irq92:
	pushq $0
	pushq $92
	jmp rk_irq_common_stub

rk_irq93:
	pushq $0
	pushq $93
	jmp rk_irq_common_stub

rk_irq94:
	pushq $0
	pushq $94
	jmp rk_irq_common_stub

rk_irq95:
	pushq $0
	pushq $95
	jmp rk_irq_common_stub

rk_irq96:
	pushq $0
	pushq $96
	jmp rk_irq_common_stub

rk_irq97:
	pushq $0
	pushq $97
	jmp rk_irq_common_stub

rk_irq98:
	pushq $0
	pushq $98
	jmp rk_irq_common_stub

rk_irq99:
	pushq $0
	pushq $99
	jmp rk_irq_common_stub

rk_irq100:
	pushq $0
	pushq $100
	jmp rk_irq_common_stub

rk_irq101:
	pushq $0
	pushq $101
	jmp rk_irq_common_stub

rk_irq102:
	pushq $0
	pushq $102
	jmp rk_irq_common_stub

rk_irq103:
	pushq $0
	pushq $103
	jmp rk_irq_common_stub

rk_irq104:
	pushq $0
	pushq $104
	jmp rk_irq_common_stub

rk_irq105:
	pushq $0
	pushq $105
	jmp rk_irq_common_stub

rk_irq106:
	pushq $0
	pushq $106
	jmp rk_irq_common_stub

rk_irq107:
	pushq $0
	pushq $107
	jmp rk_irq_common_stub

rk_irq108:
	pushq $0
	pushq $108
	jmp rk_irq_common_stub

rk_irq109:
	pushq $0
	pushq $109
	jmp rk_irq_common_stub

rk_irq110:
	pushq $0
	pushq $110
	jmp rk_irq_common_stub

rk_irq111:
	pushq $0
	pushq $111
	jmp rk_irq_common_stub

rk_irq112:
	pushq $0
	pushq $112
	jmp rk_irq_common_stub

rk_irq113:
	pushq $0
	pushq $113
	jmp rk_irq_common_stub

rk_irq114:
	pushq $0
	pushq $114
	jmp rk_irq_common_stub

rk_irq115:
	pushq $0
	pushq $115
	jmp rk_irq_common_stub

rk_irq116:
	pushq $0
	pushq $116
	jmp rk_irq_common_stub

rk_irq117:
	pushq $0
	pushq $117
	jmp rk_irq_common_stub

rk_irq118:
	pushq $0
	pushq $118
	jmp rk_irq_common_stub

rk_irq119:
	pushq $0
	pushq $119
	jmp rk_irq_common_stub

rk_irq120:
	pushq $0
	pushq $120
	jmp rk_irq_common_stub

rk_irq121:
	pushq $0
	pushq $121
	jmp rk_irq_common_stub

rk_irq122:
	pushq $0
	pushq $122
	jmp rk_irq_common_stub

rk_irq123:
	pushq $0
	pushq $123
	jmp rk_irq_common_stub

rk_irq124:
	pushq $0
	pushq $124
	jmp rk_irq_common_stub

rk_irq125:
	pushq $0
	pushq $125
	jmp rk_irq_common_stub

rk_irq126:
	pushq $0
	pushq $126
	jmp rk_irq_common_stub

rk_irq127:
	pushq $0
	pushq $127
	jmp rk_irq_common_stub

rk_irqsys:
	pushq $0
	pushq $128
	jmp rk_irq_common_stub

rk_irqipi:
	pushq $0
	pushq $0xa0
	jmp rk_irq_common_stub

rk_irqtimer:
	pushq $0
	pushq $0xa1
	jmp rk_irq_common_stub

/* interrupt service routine stub */
rk_isr_common_stub:
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rsp
	pushq %rbp
	pushq %rsi
	pushq %rdi
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	movq $0, %rax
	movw %ds, %ax
	pushq %rax

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movq %rsp, %rdi
	call rk_idt_isr_handler

	popq %rax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdi
	popq %rsi
	popq %rbp
	popq %rsp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax

	addq $16, %rsp
	iretq

/* interrupt request stub */
rk_irq_common_stub:
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rsp
	pushq %rbp
	pushq %rsi
	pushq %rdi
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	movq $0, %rax
	movw %ds, %ax
	pushq %rax

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movq %rsp, %rdi
	call rk_idt_irq_handler

	popq %rax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdi
	popq %rsi
	popq %rbp
	popq %rsp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax

	addq $16, %rsp
	iretq
