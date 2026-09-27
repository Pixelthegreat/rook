#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/console.h>
#include <rook/kernel/mmu.h>
#include <rook/kernel/acpi.h>

static rk_acpi_rsdt_t *rsdt;
static rk_acpi_xsdt_t *xsdt;

#define MAX_TABLES 128
static rk_acpi_header_t *tables[MAX_TABLES];
static size_t table_count = 0;

#define ADD_TABLE(table) if (table_count < MAX_TABLES) tables[table_count++] = table

/* validate table */
static void validate_table(rk_acpi_header_t *header) {

	void *data = (void *)header;
	size_t size = (size_t)header->length;

	uint32_t result = 0;
	for (size_t i = 0; i < size; i++)
		result += (uint32_t)(((uint8_t *)data)[i]);

	if ((result & 0xff) != 0) {

		char name[5];
		memcpy(name, header->signature, 4);
		name[4] = 0;

		rk_panic(NULL, "Can't validate ACPI '%s' table", name);
	}
}

/* map table */
static rk_acpi_header_t *map_table(uint64_t address) {

	uint64_t base = address & ~(rk_mmu_page_size - 1);
	uint64_t offset = address - base;

	void *virtual = rk_mmu_map(
			NULL, (void *)base,
			offset + sizeof(rk_acpi_header_t),
			0
	);
	if (!virtual) return NULL;

	rk_acpi_header_t *header =
		(rk_acpi_header_t *)(virtual + offset);
	uint64_t length = (uint64_t)header->length;

	uint64_t mapped = RK_ALIGN(RK_MAX(
			rk_mmu_page_size,
			offset + sizeof(rk_acpi_header_t)
	), rk_mmu_page_size);

	if (offset + length > mapped) {

		rk_mmu_unmap(virtual, mapped);

		if (!(virtual = rk_mmu_map(NULL, (void *)base, (size_t)length, 0)))
			return NULL;
		header = (rk_acpi_header_t *)(virtual + offset);
	}
	validate_table(header);

	char name[5];
	memcpy(name, header->signature, 4);
	name[4] = 0;

	rk_log_debug("Mapped '%s' table", name);

	return header;
}

/* initialize acpi */
extern void rk_acpi_init(void) {

	uint64_t base = rk_boot_info.acpi.rsdp & ~(rk_mmu_page_size - 1);
	uint64_t offset = rk_boot_info.acpi.rsdp - base;

	void *virtual = rk_mmu_map(
			NULL,
			(void *)base,
			offset + sizeof(rk_acpi_rsdp_t),
			0
	);
	rk_acpi_rsdp_t *rsdp = (rk_acpi_rsdp_t *)(virtual + offset);

	if (!!memcmp(rsdp->signature, "RSD PTR ", 8))
		rk_panic(NULL, "Invalid RSDP");

	rk_log_debug(
		"RSDP: address=0x%llx, revision=%hhu",
		rk_boot_info.acpi.rsdp,
		rsdp->revision
	);

	/* map tables from rsdt */
	if (rsdp->revision < 2) {

		rsdt = (rk_acpi_rsdt_t *)map_table(rsdp->rsdt_address);
		ADD_TABLE(&rsdt->header);

		size_t count =
			((size_t)rsdt->header.length - sizeof(rk_acpi_header_t)) /
			sizeof(uint32_t);

		for (size_t i = 0; i < count; i++)
			ADD_TABLE(map_table((uint64_t)rsdt->entries[i]));
	}

	/* map tables from xsdt */
	else {
		xsdt = (rk_acpi_xsdt_t *)map_table(rsdp->xsdt_address);
		ADD_TABLE(&xsdt->header);

		size_t count =
			((size_t)xsdt->header.length - sizeof(rk_acpi_header_t)) /
			sizeof(uint64_t);

		for (size_t i = 0; i < count; i++)
			ADD_TABLE(map_table(xsdt->entries[i]));
	}

	rk_log_info("Initialized ACPI");
}

/* get nth table of signature */
extern rk_acpi_header_t *rk_acpi_get_table(const char *signature, size_t index) {

	for (size_t i = 0; i < table_count; i++) {
		if (!memcmp(tables[i]->signature, signature, 4)) {

			if (!index) return tables[i];
			index--;
		}
	}
	return NULL;
}
