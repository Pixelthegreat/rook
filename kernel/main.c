#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/mmu.h>
#include <rook/kernel/heap.h>
#include <rook/kernel/meta.h>
#include <rook/kernel/module.h>
#include <rook/kernel/console.h>
#include <rook/kernel/acpi.h>

extern void main(void) {

	rk_boot_init();
	rk_mmu_init();
	rk_heap_init();
	rk_module_init_builtins(0, 3);
	rk_console_init();
	rk_acpi_init();
	rk_module_init_builtins(3, 100);

	rk_acpi_header_t *header = rk_acpi_get_table("APIC", 0);
	if (header) rk_log_info("Found 'APIC' table");
}
