#ifndef ROOK_KERNEL_ARCH_X86_64_CPU_H
#define ROOK_KERNEL_ARCH_X86_64_CPU_H

#include <rook/core.h>
#include <rook/kernel/cpu.h>
#include <rook/kernel/acpi.h>

/*
 * ACPI Multiple APIC Description Table
 */
#define RK_MADT_DUAL_8259 0x1

typedef struct rk_madt_entry {
	uint8_t type, size;
} __attribute__((packed)) rk_madt_entry_t;

#define RK_MADT_LAPIC 0
#define RK_MADT_IOAPIC 1
#define RK_MADT_IOAPIC_ISO 2
#define RK_MADT_IOAPIC_NMI 3
#define RK_MADT_LAPIC_NMI 4
#define RK_MADT_LAPIC_AO 5
#define RK_MADT_LAPIC2 9

typedef struct rk_madt {
	rk_acpi_header_t base;
	uint32_t lapic_address;
	uint32_t flags;
	rk_madt_entry_t entries[];
} __attribute__((packed)) rk_madt_t;

/*
 * IOAPIC
 */
#define RK_IOAPIC_COMMAND 0x0
#define RK_IOAPIC_DATA 0x4

#define RK_IOAPIC_REG_ID 0x0
#define RK_IOAPIC_REG_VERSION 0x1
#define RK_IOAPIC_REG_ARB 0x2
#define RK_IOAPIC_REG_TABLE 0x10

#define RK_IOAPIC_ID(v) (((v) >> 24) & 0xf)
#define RK_IOAPIC_VERSION(v) ((v) & 0xff)
#define RK_IOAPIC_MAX_IRQ(v) (((v) >> 16) & 0xff)

typedef struct rk_madt_ioapic {
	rk_madt_entry_t base;
	uint8_t id;
	uint8_t rsvd;
	uint32_t address;
	uint32_t gsi_base;
} __attribute__((packed)) rk_madt_ioapic_t;

/*
 * Local APIC
 */
#define RK_LAPIC_ENABLED 0x1
#define RK_LAPIC_ONLINE_CAPABLE 0x2

typedef struct rk_madt_lapic {
	rk_madt_entry_t base;
	uint8_t cpu;
	uint8_t id;
	uint32_t flags;
} __attribute__((packed)) rk_madt_lapic_t;

/*
 * Interrupt Source Overrides
 */
#define RK_MADT_ACTIVE_LOW 0x2
#define RK_MADT_TRIGGER_LEVEL 0x8

#define RK_MADT_LAPIC_ALL 0xff

typedef struct rk_madt_iso {
	rk_madt_entry_t base;
	uint8_t bus;
	uint8_t irq;
	uint32_t gsi;
	uint16_t flags;
} __attribute__((packed)) rk_madt_iso_t;

typedef struct rk_madt_nmi {
	rk_madt_entry_t base;
	uint8_t source;
	uint8_t rsvd;
	uint16_t flags;
	uint32_t gsi;
} __attribute__((packed)) rk_madt_nmi_t;

typedef struct rk_madt_lapic_nmi {
	rk_madt_entry_t base;
	uint8_t id;
	uint16_t flags;
	uint8_t lint;
} __attribute__((packed)) rk_madt_lapic_nmi_t;

typedef struct rk_madt_lapic_ao {
	rk_madt_entry_t base;
	uint16_t rsvd;
	uint64_t address;
} __attribute__((packed)) rk_madt_lapic_ao_t;

/*
 * IOAPIC IRQ flags
 */
#define RK_IOAPIC_IRQ_FIXED 0x0
#define RK_IOAPIC_IRQ_LOWEST_PRIORITY 0x1
#define RK_IOAPIC_IRQ_SMI 0x2
#define RK_IOAPIC_IRQ_NMI 0x4
#define RK_IOAPIC_IRQ_INIT 0x5
#define RK_IOAPIC_IRQ_EXTINT 0x7
#define RK_IOAPIC_IRQ_DELIVERY(v) ((v) << 8)
#define RK_IOAPIC_IRQ_LOGICAL 0x400
#define RK_IOAPIC_IRQ_ACTIVE_LOW 0x1000
#define RK_IOAPIC_IRQ_TRIGGER_LEVEL 0x8000
#define RK_IOAPIC_IRQ_MASK 0x10000
#define RK_IOAPIC_IRQ_DESTINATION(v) ((v) << 24)

/*
 * Local APIC
 */
#define RK_LAPIC_REG_ID 0x20
#define RK_LAPIC_REG_VERSION 0x30
#define RK_LAPIC_REG_TPR 0x80
#define RK_LAPIC_REG_APR 0x90
#define RK_LAPIC_REG_PPR 0xa0
#define RK_LAPIC_REG_EOI 0xb0
#define RK_LAPIC_REG_RRD 0xc0
#define RK_LAPIC_REG_LOGICAL_DESTINATION 0xd0
#define RK_LAPIC_REG_DESTINATION_FORMAT 0xe0
#define RK_LAPIC_REG_SPURIOUS_INTERRUPT 0xf0
#define RK_LAPIC_REG_ISR 0x100
#define RK_LAPIC_REG_TMR 0x180
#define RK_LAPIC_REG_IRR 0x200
#define RK_LAPIC_REG_ERROR_STATUS 0x280
#define RK_LAPIC_REG_LVT_CMCI 0x2f0
#define RK_LAPIC_REG_ICR0 0x300
#define RK_LAPIC_REG_ICR1 0x310
#define RK_LAPIC_REG_LVT_TIMER 0x320
#define RK_LAPIC_REG_LVT_THERMAL_SENSOR 0x330
#define RK_LAPIC_REG_LVT_PERF_COUNTER 0x340
#define RK_LAPIC_REG_LVT_LINT0 0x350
#define RK_LAPIC_REG_LVT_LINT1 0x360
#define RK_LAPIC_REG_LVT_ERROR 0x370
#define RK_LAPIC_REG_INITIAL_COUNT 0x380
#define RK_LAPIC_REG_CURRENT_COUNT 0x390
#define RK_LAPIC_REG_DIVIDE_CONFIG 0x3e0
#define RK_LAPIC_REG_END 0x400

#define RK_LAPIC_CMD_DELIVERY_NORMAL 0x0
#define RK_LAPIC_CMD_DELIVERY_LOWEST 0x100
#define RK_LAPIC_CMD_DELIVERY_SMI 0x200
#define RK_LAPIC_CMD_DELIVERY_NMI 0x400
#define RK_LAPIC_CMD_DELIVERY_INIT 0x500
#define RK_LAPIC_CMD_DELIVERY_SIPI 0x600
#define RK_LAPIC_CMD_DELIVERY_STATUS 0x1000
#define RK_LAPIC_CMD_LOGICAL_DESTINATION 0x800
#define RK_LAPIC_CMD_NO_INIT_LEVEL_DEASSERT 0x4000
#define RK_LAPIC_CMD_INIT_LEVEL_DEASSERT 0x8000
#define RK_LAPIC_CMD_DESTINATION(id) ((id) << 24)

#define RK_LAPIC_LVT_INT_PENDING 0x1000
#define RK_LAPIC_LVT_MASK 0x10000
#define RK_LAPIC_LVT_PERIODIC 0x20000

#define RK_LAPIC_ID(value) ((value) >> 24)

/*
 * Output to IOAPIC register
 */
extern void rk_ioapic_outd(
		volatile uint32_t *base,
		uint32_t offset,
		uint32_t data
);

/*
 * Input from IOAPIC register
 */
extern uint32_t rk_ioapic_ind(
		volatile uint32_t *base,
		uint32_t offset
);

/*
 * Output to local APIC register
 */
extern void rk_lapic_outd(
		uint32_t offset,
		uint32_t data
);

/*
 * Input from local APIC register
 */
extern uint32_t rk_lapic_ind(uint32_t offset);

/*
 * Enable vector
 */
extern void rk_ioapic_enable(
		size_t vector,
		uint32_t destination
);

/*
 * Disable vector
 */
extern void rk_ioapic_disable(size_t vector);

/*
 * Acknowledge interrupt
 */
extern void rk_lapic_acknowledge(void);

/*
 * AP / SMP entry point
 */
extern void rk_ap_entry(void);

#endif /* ROOK_KERNEL_ARCH_X86_64_CPU_H */
