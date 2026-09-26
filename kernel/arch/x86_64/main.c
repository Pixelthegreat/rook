#include <rook/core.h>
#include <rook/kernel/console.h>
#include <rook/kernel/arch/x86_64/gdt.h>

/* entry point */
extern void main(void);

extern void arch_main(void) {

	gdt_load(NULL);
	rk_log_debug("Initialized GDT");

	main();

	while (1) asm volatile("hlt");
}
