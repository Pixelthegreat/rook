/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ROOK_KERNEL_ACPI_H
#define ROOK_KERNEL_ACPI_H

#include <rook/core.h>

/*
 * Root System Description Pointer (RSDP)
 */
typedef struct rk_acpi_rsdp {
	uint8_t signature[8];
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t revision;
	uint32_t rsdt_address;

	uint32_t length;
	uint64_t xsdt_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
} __attribute__((packed)) rk_acpi_rsdp_t;

/*
 * Header
 */
typedef struct rk_acpi_header {
	uint8_t signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} __attribute__((packed)) rk_acpi_header_t;

/*
 * Root System Description Table
 */
typedef struct rk_acpi_rsdt {
	rk_acpi_header_t header;
	uint32_t entries[];
} __attribute__((packed)) rk_acpi_rsdt_t;

typedef struct rk_acpi_xsdt {
	rk_acpi_header_t header;
	uint64_t entries[];
} __attribute__((packed)) rk_acpi_xsdt_t;

/*
 * Initialize ACPI
 */
extern void rk_acpi_init(void);

/*
 * Get nth table of signature
 */
extern rk_acpi_header_t *rk_acpi_get_table(const char *signature, size_t index);

#endif /* ROOK_KERNEL_ACPI_H */
