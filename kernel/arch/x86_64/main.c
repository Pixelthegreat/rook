#include <rook/core.h>
#include <rook/kernel/console.h>
#include <rook/kernel/arch/x86_64/gdt.h>
#include <rook/kernel/arch/x86_64/idt.h>

/* entry point */
extern void main(void);

extern void arch_main(void) {

	rk_gdt_load(NULL);
	rk_idt_init();

	rk_log_debug("Initialized GDT");

	main();

	while (1) asm volatile("hlt");
}
