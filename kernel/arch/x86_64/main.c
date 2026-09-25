#include <rook/core.h>
#include <rook/kernel/console.h>
#include <rook/kernel/arch/x86_64/isa.h>

#define THR 0
#define RBR 0
#define DLL 0
#define IER 1
#define DLH 1
#define IIR 2
#define FCR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6
#define SR 7
#define PORT 0x3f8

/* initialize console */
extern void rk_console_init(void) {

	isa_outb(PORT+FCR, 0);
	isa_outb(PORT+LCR, 0x80);
	isa_outb(PORT+DLL, 12);
	isa_outb(PORT+DLH, 0);
	isa_outb(PORT+LCR, 0x03);
	isa_outb(PORT+MCR, 0);
	isa_outb(PORT+IER, 1);

	isa_inb(PORT+LSR);
}

/* print character to console */
extern void rk_console_printc(char c) {

	for (int i = 0; !(isa_inb(PORT+LSR) & 0x20) && i < 65535; i++);
	isa_outb(PORT+THR, (uint8_t)c);

	if (c == '\n') rk_console_printc('\r');
}

/* print message to console */
extern void rk_console_prints(const char *message) {

	while (*message) rk_console_printc(*message++);
}

/* entry point */
extern void main(void);

extern void arch_main(void) {

	main();

	while (1) asm volatile("hlt");
}
