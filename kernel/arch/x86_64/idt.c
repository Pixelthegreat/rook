#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/console.h>
#include <rook/kernel/arch/x86_64/gdt.h>
#include <rook/kernel/arch/x86_64/idt.h>

#define MAX_INTERRUPTS 256

static rk_idt_gate_t idt[MAX_INTERRUPTS];
static __attribute__((used)) rk_idtr_t idtr = {
	.size = sizeof(idt),
	.address = (uint64_t)idt,
};
static rk_idt_handler_t handlers[MAX_INTERRUPTS];

/* load idt */
extern void rk_idt_load(void) {

	asm volatile("lidtq (idtr)");
}

/* initialize */
extern void rk_idt_init(void) {

	asm volatile("cli");

	rk_idt_set_gate(0, rk_isr0, RK_IDT_GATE_TRAP); /* divide error */
	rk_idt_set_gate(1, rk_isr1, RK_IDT_GATE_TRAP); /* debug exception */
	rk_idt_set_gate(2, rk_isr2, RK_IDT_GATE_INTERRUPT); /* non-maskable interrupt */
	rk_idt_set_gate(3, rk_isr3, RK_IDT_GATE_TRAP); /* breakpoint */
	rk_idt_set_gate(4, rk_isr4, RK_IDT_GATE_TRAP); /* overflow */
	rk_idt_set_gate(5, rk_isr5, RK_IDT_GATE_TRAP); /* bound range exceeded */
	rk_idt_set_gate(6, rk_isr6, RK_IDT_GATE_TRAP); /* undefined opcode */
	rk_idt_set_gate(7, rk_isr7, RK_IDT_GATE_TRAP); /* no math coprocessor */
	rk_idt_set_gate(8, rk_isr8, RK_IDT_GATE_INTERRUPT); /* double fault */
	rk_idt_set_gate(9, rk_isr9, RK_IDT_GATE_TRAP); /* reserved */
	rk_idt_set_gate(10, rk_isr10, RK_IDT_GATE_TRAP); /* invalid tss */
	rk_idt_set_gate(11, rk_isr11, RK_IDT_GATE_TRAP); /* segment not present */
	rk_idt_set_gate(12, rk_isr12, RK_IDT_GATE_TRAP); /* stack-segment fault */
	rk_idt_set_gate(13, rk_isr13, RK_IDT_GATE_TRAP); /* general protection fault */
	rk_idt_set_gate(14, rk_isr14, RK_IDT_GATE_TRAP); /* page fault */
	rk_idt_set_gate(15, rk_isr15, RK_IDT_GATE_TRAP); /* reserved */
	rk_idt_set_gate(16, rk_isr16, RK_IDT_GATE_TRAP); /* math fault */
	rk_idt_set_gate(17, rk_isr17, RK_IDT_GATE_TRAP); /* alignment check */
	rk_idt_set_gate(18, rk_isr18, RK_IDT_GATE_INTERRUPT); /* machine check */
	rk_idt_set_gate(19, rk_isr19, RK_IDT_GATE_TRAP); /* simd floating point exception */
	rk_idt_set_gate(20, rk_isr20, RK_IDT_GATE_TRAP); /* virtualization exception */
	rk_idt_set_gate(21, rk_isr21, RK_IDT_GATE_TRAP); /* control protection exception */
	rk_idt_set_gate(22, rk_isr22, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(23, rk_isr23, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(24, rk_isr24, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(25, rk_isr25, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(26, rk_isr26, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(27, rk_isr27, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(28, rk_isr28, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(29, rk_isr29, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(30, rk_isr30, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(31, rk_isr31, RK_IDT_GATE_INTERRUPT);

	/* hardware irqs */
	rk_idt_set_gate(48, rk_irq48, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(49, rk_irq49, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(50, rk_irq50, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(51, rk_irq51, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(52, rk_irq52, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(53, rk_irq53, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(54, rk_irq54, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(55, rk_irq55, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(56, rk_irq56, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(57, rk_irq57, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(58, rk_irq58, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(59, rk_irq59, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(60, rk_irq60, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(61, rk_irq61, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(62, rk_irq62, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(63, rk_irq63, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(64, rk_irq64, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(65, rk_irq65, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(66, rk_irq66, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(67, rk_irq67, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(68, rk_irq68, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(69, rk_irq69, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(70, rk_irq70, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(71, rk_irq71, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(72, rk_irq72, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(73, rk_irq73, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(74, rk_irq74, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(75, rk_irq75, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(76, rk_irq76, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(77, rk_irq77, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(78, rk_irq78, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(79, rk_irq79, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(80, rk_irq80, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(81, rk_irq81, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(82, rk_irq82, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(83, rk_irq83, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(84, rk_irq84, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(85, rk_irq85, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(86, rk_irq86, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(87, rk_irq87, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(88, rk_irq88, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(89, rk_irq89, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(90, rk_irq90, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(91, rk_irq91, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(92, rk_irq92, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(93, rk_irq93, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(94, rk_irq94, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(95, rk_irq95, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(96, rk_irq96, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(97, rk_irq97, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(98, rk_irq98, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(99, rk_irq99, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(100, rk_irq100, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(101, rk_irq101, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(102, rk_irq102, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(103, rk_irq103, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(104, rk_irq104, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(105, rk_irq105, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(106, rk_irq106, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(107, rk_irq107, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(108, rk_irq108, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(109, rk_irq109, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(110, rk_irq110, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(111, rk_irq111, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(112, rk_irq112, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(113, rk_irq113, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(114, rk_irq114, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(115, rk_irq115, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(116, rk_irq116, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(117, rk_irq117, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(118, rk_irq118, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(119, rk_irq119, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(120, rk_irq120, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(121, rk_irq121, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(122, rk_irq122, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(123, rk_irq123, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(124, rk_irq124, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(125, rk_irq125, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(126, rk_irq126, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(127, rk_irq127, RK_IDT_GATE_INTERRUPT);

	rk_idt_set_gate(128, rk_irqsys, RK_IDT_GATE_INTERRUPT | RK_IDT_GATE_DPL(3));

	/* miscellaneous */
	rk_idt_set_gate(RK_IDT_IPI_VECTOR, rk_irqipi, RK_IDT_GATE_INTERRUPT);
	rk_idt_set_gate(RK_IDT_TIMER_VECTOR, rk_irqtimer, RK_IDT_GATE_INTERRUPT);

	rk_idt_load();
	rk_log_info("Initialized IDT");
}

/* set gate handler */
extern void rk_idt_set_gate(uint64_t index, void *procedure, uint8_t type) {

	if (index >= MAX_INTERRUPTS)
		return;

	uint64_t offset = (uint64_t)procedure;

	idt[index].offset_low = (uint16_t)(offset & 0xffff);
	idt[index].selector = RK_GDT_INDEX(RK_GDT_ENTRY_KERNEL_CODE);
	idt[index].ist_rsvd = 0;
	idt[index].flags = RK_IDT_GATE_P | type;
	idt[index].offset_mid = (uint16_t)((offset >> 16) & 0xffff);
	idt[index].offset_high = (uint32_t)((offset >> 32) & 0xffffffff);
	idt[index].rsvd = 0;
}

/* isr handler */
extern void rk_idt_isr_handler(rk_idt_registers_t *registers) {

	if (handlers[registers->index])
		handlers[registers->index](registers);
	else
		rk_panic(
			registers,
			"CPU exception %llu (code %llu)",
			registers->index,
			registers->code
		);
}

/* irq handler */
extern void rk_idt_irq_handler(rk_idt_registers_t *registers) {

	if (handlers[registers->index])
		handlers[registers->index](registers);
}

/* set interrupt handler */
extern void rk_idt_set_handler(uint64_t index, rk_idt_handler_t handler) {

	if (index >= MAX_INTERRUPTS)
		return;
	handlers[index] = handler;
}

/* print panic context */
static char panic_buffer[1024];

extern void rk_print_panic_context(void *context) {

	rk_idt_registers_t *registers = (rk_idt_registers_t *)context;

	snprintf(
		panic_buffer, sizeof(panic_buffer),
		"  \e[41m\e[91mRDI   =0x%16llx  RSI    =0x%16llx\e[49m\e[39m\n"
		"  \e[41m\e[91mRBP   =0x%16llx  RSP    =0x%16llx\e[49m\e[39m\n"
		"  \e[41m\e[91mRDX   =0x%16llx  RCX    =0x%16llx\e[49m\e[39m\n"
		"  \e[41m\e[91mRBX   =0x%16llx  RAX    =0x%16llx\e[49m\e[39m\n"
		"  \e[41m\e[91mRIP   =0x%16llx  CS     =0x%16llx\e[49m\e[39m\n"
		"  \e[41m\e[91mRFLAGS=0x%16llx  USERRSP=0x%16llx\e[49m\e[39m\n"
		"  \e[41m\e[91mSS    =0x%16llx  DS     =0x%16llx\e[49m\e[39m\n",
		registers->rdi, registers->rsi,
		registers->rbp, registers->rsp,
		registers->rdx, registers->rcx,
		registers->rbx, registers->rax,
		registers->rip, registers->cs,
		registers->rflags, registers->userrsp,
		registers->ss, registers->ds
	);
	rk_console_prints(panic_buffer);
}
