#include <rook/core.h>
#include <rook/kernel/console.h>
#include <rook/kernel/arch/x86_64/isa.h>

/* entry point */
extern void main(void);

extern void arch_main(void) {

	main();

	while (1) asm volatile("hlt");
}
