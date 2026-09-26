#ifndef ROOK_KERNEL_ARCH_X86_64_IDT_H
#define ROOK_KERNEL_ARCH_X86_64_IDT_H

#include <rook/core.h>

#define RK_IDT_IPI_VECTOR 0xa0
#define RK_IDT_TIMER_VECTOR 0xa1

/*
 * IDT register
 */
typedef struct rk_idtr {
	uint16_t size;
	uint64_t address;
} __attribute__((packed)) rk_idtr_t;

/*
 * IDT entry / gate descriptor
 */
typedef struct rk_idt_gate {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t ist_rsvd;
	uint8_t flags;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t rsvd;
} __attribute__((packed)) rk_idt_gate_t;

#define RK_IDT_GATE_INTERRUPT 0xe
#define RK_IDT_GATE_TRAP 0xf
#define RK_IDT_GATE_DPL(i) ((i) << 5)
#define RK_IDT_GATE_P 0x80

/*
 * Registers
 */
typedef struct rk_idt_registers {
	uint64_t ds;
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rdi, rsi, rbp, rsp, rdx, rcx, rbx, rax;
	uint64_t index, code;
	uint64_t rip, cs, rflags, userrsp, ss;
} rk_idt_registers_t;

typedef void (*rk_idt_handler_t)(rk_idt_registers_t *);

#define RK_IDT_ISR_OFFSET 0

/*
 * Load IDT
 */
extern void rk_idt_load(void);
extern void rk_idt_init(void);

/*
 * Set gate handler
 */
extern void rk_idt_set_gate(uint64_t index, void *procedure, uint8_t type);

/*
 * Interrupt routines
 */
extern void rk_idt_isr_handler(rk_idt_registers_t *registers);
extern void rk_idt_irq_handler(rk_idt_registers_t *registers);

/*
 * Set interrupt handler
 */
extern void rk_idt_set_handler(uint64_t index, rk_idt_handler_t handler);

/*
 * ISRs and IRQs
 */
extern void rk_isr0();
extern void rk_isr1();
extern void rk_isr2();
extern void rk_isr3();
extern void rk_isr4();
extern void rk_isr5();
extern void rk_isr6();
extern void rk_isr7();
extern void rk_isr8();
extern void rk_isr9();
extern void rk_isr10();
extern void rk_isr11();
extern void rk_isr12();
extern void rk_isr13();
extern void rk_isr14();
extern void rk_isr15();
extern void rk_isr16();
extern void rk_isr17();
extern void rk_isr18();
extern void rk_isr19();
extern void rk_isr20();
extern void rk_isr21();
extern void rk_isr22();
extern void rk_isr23();
extern void rk_isr24();
extern void rk_isr25();
extern void rk_isr26();
extern void rk_isr27();
extern void rk_isr28();
extern void rk_isr29();
extern void rk_isr30();
extern void rk_isr31();

extern void rk_irq48();
extern void rk_irq49();
extern void rk_irq50();
extern void rk_irq51();
extern void rk_irq52();
extern void rk_irq53();
extern void rk_irq54();
extern void rk_irq55();
extern void rk_irq56();
extern void rk_irq57();
extern void rk_irq58();
extern void rk_irq59();
extern void rk_irq60();
extern void rk_irq61();
extern void rk_irq62();
extern void rk_irq63();
extern void rk_irq64();
extern void rk_irq65();
extern void rk_irq66();
extern void rk_irq67();
extern void rk_irq68();
extern void rk_irq69();
extern void rk_irq70();
extern void rk_irq71();
extern void rk_irq72();
extern void rk_irq73();
extern void rk_irq74();
extern void rk_irq75();
extern void rk_irq76();
extern void rk_irq77();
extern void rk_irq78();
extern void rk_irq79();
extern void rk_irq80();
extern void rk_irq81();
extern void rk_irq82();
extern void rk_irq83();
extern void rk_irq84();
extern void rk_irq85();
extern void rk_irq86();
extern void rk_irq87();
extern void rk_irq88();
extern void rk_irq89();
extern void rk_irq90();
extern void rk_irq91();
extern void rk_irq92();
extern void rk_irq93();
extern void rk_irq94();
extern void rk_irq95();
extern void rk_irq96();
extern void rk_irq97();
extern void rk_irq98();
extern void rk_irq99();
extern void rk_irq100();
extern void rk_irq101();
extern void rk_irq102();
extern void rk_irq103();
extern void rk_irq104();
extern void rk_irq105();
extern void rk_irq106();
extern void rk_irq107();
extern void rk_irq108();
extern void rk_irq109();
extern void rk_irq110();
extern void rk_irq111();
extern void rk_irq112();
extern void rk_irq113();
extern void rk_irq114();
extern void rk_irq115();
extern void rk_irq116();
extern void rk_irq117();
extern void rk_irq118();
extern void rk_irq119();
extern void rk_irq120();
extern void rk_irq121();
extern void rk_irq122();
extern void rk_irq123();
extern void rk_irq124();
extern void rk_irq125();
extern void rk_irq126();
extern void rk_irq127();

extern void rk_irqsys();

extern void rk_irqipi();
extern void rk_irqtimer();

#endif /* ROOK_KERNEL_ARCH_X86_64_IDT_H */
