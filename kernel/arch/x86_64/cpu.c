#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/console.h>
#include <rook/kernel/mmu.h>
#include <rook/kernel/heap.h>
#include <rook/kernel/cpu.h>
#include <rook/kernel/arch/x86_64/isa.h>
#include <rook/kernel/arch/x86_64/gdt.h>
#include <rook/kernel/arch/x86_64/idt.h>
#include <rook/kernel/arch/x86_64/cpu.h>

extern uint64_t _rk_ap_start;
extern uint64_t rk_mmu_main_cr3;

#define AP_GDT_OFFSET 0xc0
#define AP_DATA_OFFSET 0x200

#define AP_DATA_STATUS 0
#define AP_DATA_STACK_POINTER 1
#define AP_DATA_ENTRY_POINT 2
#define AP_DATA_CR3_VALUE 3

#define NEW_STACK_SIZE 0x20000

#define MASTER_COMMAND 0x20
#define MASTER_DATA 0x21

#define SLAVE_COMMAND 0xa0
#define SLAVE_DATA 0xa1

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define EOI 0x20

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUFFERED_SLAVE 0x08
#define ICW4_BUFFERED_MASTER 0x0c
#define ICW4_SFNM 0x10

#define CASCADE_IRQ 2

static size_t ap_index = 0;
static bool ap_initialized = false;

/* ioapic info */
#define MAX_IOAPICS 128

static struct {
	volatile uint32_t *address;
	uint32_t id;
	size_t base;
	size_t count;
	uint32_t gsi_base;
} ioapics[MAX_IOAPICS];
static size_t ioapic_count;

/* local apic info */
#define MAX_LAPICS 128

static struct {
	rk_madt_lapic_t *table;
	uint32_t id;
	uint64_t tps; /* ticks per second for apictimer */
} lapics[MAX_LAPICS];
static size_t lapic_count;
static size_t lapic2cpu[256];

static uint64_t lapic_physical;
static volatile uint32_t *lapic_address;

static rk_device_t *main_cpu;

/* other tables */
#define MAX_OTHERS 256

static rk_madt_entry_t *others[MAX_OTHERS];
static size_t other_count;

static size_t irq_map[256];
static bool init = false;

/* delays */
static bool timer_called = false;
static rk_spinlock_t timer_lock, timer_irq_lock;

static void timer_irq(rk_idt_registers_t *registers) {

	rk_lapic_acknowledge();

	rk_spinlock_acquire(&timer_irq_lock);
	timer_called = true;
	rk_spinlock_release(&timer_irq_lock);
}

static void wait_div(uint32_t div) {

	rk_isa_outb(0x43, (3 << 4));
	rk_isa_outb(0x40, (uint8_t)(div & 0xff));
	rk_isa_outb(0x40, (uint8_t)((div >> 8) & 0xff));

	while (!timer_called) asm volatile("hlt");
	timer_called = false;
}

static void wait_div_direct(uint32_t div) {

	timer_called = false;
	rk_idt_set_handler(irq_map[0], timer_irq);
	wait_div(div);
	rk_idt_set_handler(irq_map[0], NULL);
}

static void wait_ms(uint32_t ms) {

	timer_called = false;
	rk_idt_set_handler(irq_map[0], timer_irq);

	while (ms >= 10) {

		wait_div(11931);
		ms -= 10;
	}
	wait_div(1193 * ms);

	rk_idt_set_handler(irq_map[0], NULL);
}

static void wait_us(uint32_t us) {

	timer_called = false;
	rk_idt_set_handler(irq_map[0], timer_irq);

	while (us >= 100) {

		wait_div(120);
		us -= 100;
	}
	while (us >= 10) {

		wait_div(10);
		us -= 10;
	}
	rk_idt_set_handler(irq_map[0], NULL);
}

/* get local cpu */
static rk_device_t *get_local_cpu(void) {

	uint32_t lapic_id = RK_LAPIC_ID(rk_lapic_ind(RK_LAPIC_REG_ID));
	if (lapic_id < 256) return rk_cpu_get(lapic2cpu[lapic_id]);

	return NULL;
}

/* inter-processor interrupts */
static void ipi_irq(rk_idt_registers_t *registers) {

	rk_device_t *device = get_local_cpu();
	if (!device) return;

	rk_cpu_device_t *cpu = RK_CPU_DEVICE(device);

	rk_lapic_acknowledge();
	rk_cpu_signaled(device);
}

/* TODO: local apic timer irq */
static void lapic_timer_irq(rk_idt_registers_t *registers) {}

/* calibrate local apic timer */
static void calibrate_timer(size_t id) {

	rk_spinlock_acquire(&timer_lock);
	rk_ioapic_enable(irq_map[0], lapics[id].id);

	rk_lapic_outd(RK_LAPIC_REG_DIVIDE_CONFIG, 0b1000); /* divide frequency by 32 */
	rk_lapic_outd(RK_LAPIC_REG_LVT_TIMER, RK_IDT_TIMER_VECTOR);
	rk_lapic_outd(RK_LAPIC_REG_INITIAL_COUNT, 0xffffffff);

	wait_div_direct(59655);
	uint32_t count = 0xffffffff - rk_lapic_ind(RK_LAPIC_REG_CURRENT_COUNT);
	lapics[id].tps = (uint64_t)count * 20;

	rk_spinlock_release(&timer_lock);
	rk_log_debug("Timer: id=%u, tps=%llu", lapics[id].id, lapics[id].tps);
}

/* initialize all cpus */
static inline void wait(void) {

	rk_isa_outb(0x80, 0);
}

static void wait_for_delivery(void) {

	while (rk_lapic_ind(RK_LAPIC_REG_ICR0) & RK_LAPIC_CMD_DELIVERY_STATUS)
		asm volatile("pause": : : "memory");
}

extern void rk_cpu_init_all(void) {

	rk_idt_set_handler(RK_IDT_IPI_VECTOR, ipi_irq);
	rk_idt_set_handler(RK_IDT_TIMER_VECTOR, lapic_timer_irq);

	/* mask interrupts for the old pic */
	rk_isa_outb(MASTER_COMMAND, ICW1_INIT | ICW1_ICW4);
	wait();
	rk_isa_outb(SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4);
	wait();

	rk_isa_outb(MASTER_DATA, 32);
	wait();
	rk_isa_outb(SLAVE_DATA, 40);
	wait();

	rk_isa_outb(MASTER_DATA, 1 << CASCADE_IRQ);
	wait();
	rk_isa_outb(SLAVE_DATA, 2);
	wait();

	rk_isa_outb(MASTER_DATA, ICW4_8086);
	wait();
	rk_isa_outb(SLAVE_DATA, ICW4_8086);
	wait();

	rk_isa_outb(MASTER_DATA, 0xff);
	wait();
	rk_isa_outb(SLAVE_DATA, 0xff);
	wait();

	rk_log_debug("Masked 8259 PIC");

	/* locate apic table */
	rk_acpi_header_t *header = rk_acpi_get_table("APIC", 0);
	if (!header) rk_panic(NULL, "No APIC table found");

	rk_madt_t *madt = (rk_madt_t *)header;
	lapic_physical = (uint64_t)madt->lapic_address;

	size_t length = (size_t)header->length;
	size_t position = sizeof(rk_madt_t);

	/* locate and configure ioapics */
	size_t interrupt_base = 48;
	while (position < length) {

		rk_madt_entry_t *entry = (rk_madt_entry_t *)
				((void *)madt + position);
		position += (size_t)entry->size;

		/* unrelated tables */
		if ((entry->type == RK_MADT_IOAPIC_ISO ||
		     entry->type == RK_MADT_IOAPIC_NMI ||
		     entry->type == RK_MADT_LAPIC_NMI ||
		     entry->type == RK_MADT_LAPIC_AO) &&
		    other_count < MAX_OTHERS) {

			others[other_count++] = entry;
			continue;
		}

		/* local apics */
		else if (entry->type == RK_MADT_LAPIC && lapic_count < MAX_LAPICS) {

			rk_madt_lapic_t *lapic = (rk_madt_lapic_t *)entry;

			lapics[lapic_count].table = lapic;
			lapics[lapic_count].id = lapic->id;
			lapic_count++;

			rk_log_debug(
				"Local APIC: id=%hhu, cpu=%hhu",
				lapic->id, lapic->cpu
			);
			continue;
		}

		/* address override (TODO: fix?) */
		else if (entry->type == RK_MADT_LAPIC_AO) {

			rk_madt_lapic_ao_t *ao = (rk_madt_lapic_ao_t *)entry;
			lapic_physical = ao->address;
			continue;
		}
		else if (entry->type != RK_MADT_IOAPIC ||
			 ioapic_count >= MAX_IOAPICS)
			continue;

		/* map ioapic */
		rk_madt_ioapic_t *info = (rk_madt_ioapic_t *)entry;

		uint32_t base = info->address & ~(rk_mmu_page_size - 1);
		uint32_t offset = info->address - base;

		void *mapping = rk_mmu_map(
				NULL, (void *)(uint64_t)base,
				offset + 32, 0
		);
		if (!mapping) rk_panic(NULL, "Can't map I/O APIC");

		/* add ioapic to list */
		size_t i = ioapic_count++;

		ioapics[i].address = (uint32_t *)(mapping + offset);
		ioapics[i].id = (uint32_t)info->id;

		uint32_t v = rk_ioapic_ind(ioapics[i].address, RK_IOAPIC_REG_VERSION);

		ioapics[i].base = interrupt_base;
		ioapics[i].count = (size_t)(RK_IOAPIC_MAX_IRQ(v) + 1);
		ioapics[i].gsi_base = info->gsi_base;

		interrupt_base += ioapics[i].count;

		/* configure ioapic to respond to chosen interrupt vectors */
		for (size_t j = 0; j < ioapics[i].count; j++) {

			uint32_t offset = RK_IOAPIC_REG_TABLE + (uint32_t)j * 2;
			volatile uint32_t *address = ioapics[i].address;

			uint32_t vector = (uint32_t)(ioapics[i].base + j);
			if (vector >= 128)
				rk_ioapic_outd(
					address, offset,
					RK_IOAPIC_IRQ_MASK
				);
			else {
				rk_ioapic_outd(
					address, offset,
					vector | RK_IOAPIC_IRQ_MASK
				);
				rk_ioapic_outd(
					address, offset+1,
					RK_IOAPIC_IRQ_DESTINATION(ioapics[i].id)
				);
			}
		}

		rk_log_debug(
			"I/O APIC: id=%u, mapping=%p, start=%zu, end=%zu",
			ioapics[i].id,
			ioapics[i].address,
			ioapics[i].base,
			ioapics[i].base + ioapics[i].count
		);
	}
	if (!ioapic_count)
		rk_panic(NULL, "Couldn't locate any I/O APICs");

	/* configure irqs according to interrupt source overrides */
	for (size_t i = 0; i < other_count; i++) {

		rk_madt_entry_t *entry = others[i];

		/* interrupt source override */
		if (entry->type == RK_MADT_IOAPIC_ISO) {

			rk_madt_iso_t *iso = (rk_madt_iso_t *)entry;

			size_t j = 0;
			for (; j < ioapic_count; j++) {
				if (iso->gsi >= ioapics[j].gsi_base &&
				    iso->gsi < ioapics[j].gsi_base + (uint32_t)ioapics[j].count)
					break;
			}
			if (j >= ioapic_count)
				continue;

			volatile uint32_t *ioapic = ioapics[j].address;
			uint32_t index = iso->gsi - ioapics[j].gsi_base;

			/* determine flags */
			uint32_t flags = rk_ioapic_ind(
					ioapic,
					RK_IOAPIC_REG_TABLE + index * 2
			) & 0xff;

			if (iso->flags & RK_MADT_ACTIVE_LOW)
				flags |= RK_IOAPIC_IRQ_ACTIVE_LOW;
			if (iso->flags & RK_MADT_TRIGGER_LEVEL)
				flags |= RK_IOAPIC_IRQ_TRIGGER_LEVEL;
			flags |= RK_IOAPIC_IRQ_MASK;

			rk_ioapic_outd(ioapic, RK_IOAPIC_REG_TABLE + index * 2, flags);

			size_t vector = ioapics[j].base + (size_t)index;
			irq_map[iso->irq] = vector;

			rk_log_debug("Legacy IRQ: id=%hhu, mapping=%zu", iso->irq, vector);
		}

		/* non-maskable interrupt */
		else if (entry->type == RK_MADT_IOAPIC_NMI) {

			rk_madt_nmi_t *nmi = (rk_madt_nmi_t *)entry;

			size_t j = 0;
			for (; j < ioapic_count; j++) {
				if (nmi->gsi >= ioapics[j].gsi_base &&
				    nmi->gsi < ioapics[j].gsi_base + (uint32_t)ioapics[j].count)
					break;
			}
			if (j >= ioapic_count)
				continue;

			volatile uint32_t *ioapic = ioapics[j].address;
			uint32_t index = nmi->gsi - ioapics[j].gsi_base;

			/* determine flags */
			uint32_t flags = rk_ioapic_ind(
					ioapic,
					RK_IOAPIC_REG_TABLE + index * 2
			) & 0xff;

			if (nmi->flags & RK_MADT_ACTIVE_LOW)
				flags |= RK_IOAPIC_IRQ_ACTIVE_LOW;
			if (nmi->flags & RK_MADT_TRIGGER_LEVEL)
				flags |= RK_IOAPIC_IRQ_TRIGGER_LEVEL;

			rk_ioapic_outd(ioapic, RK_IOAPIC_REG_TABLE + index * 2, flags);

			size_t vector = ioapics[j].base + (size_t)index;
			rk_log_debug("NMI: id=%hhu, mapping=%zu", nmi->source, vector);
		}
	}

	/* map local apic */
	uint64_t base = lapic_physical & ~(rk_mmu_page_size - 1);
	uint64_t offset = lapic_physical - base;

	void *address = rk_mmu_map(
			NULL, (void *)base,
			offset + RK_LAPIC_REG_END,
			0
	);
	if (!address)
		rk_panic(NULL, "Can't map local APIC");

	lapic_address = (volatile uint32_t *)(address + offset);

	uint32_t lapic_id = RK_LAPIC_ID(rk_lapic_ind(RK_LAPIC_REG_ID));
	uint32_t lapic_version = rk_lapic_ind(RK_LAPIC_REG_VERSION);

	rk_log_debug(
		"Local APIC: id=%u, mapping=%p, version=0x%x",
		lapic_id, lapic_address, lapic_version
	);
	rk_ioapic_enable(irq_map[0], lapic_id); /* timer */

	/* configure local apic */
	rk_lapic_outd(RK_LAPIC_REG_SPURIOUS_INTERRUPT, 0x1ff);
	asm volatile("sti");

	/* copy bootstrap code (NEVER DO THIS ANYWHERE ELSE) */
	void *old_buffer = malloc(rk_mmu_page_size);

	void *ap_bootstrap = rk_mmu_map(
			(void *)0x1000, (void *)0x1000,
			rk_mmu_page_size, 0
	);
	memcpy(old_buffer, ap_bootstrap, rk_mmu_page_size);
	memcpy(ap_bootstrap, &_rk_ap_start, rk_mmu_page_size);

	memcpy(ap_bootstrap + AP_GDT_OFFSET, rk_gdt, sizeof(rk_gdt_entry_t) * 3);

	uint64_t *ap_data = (uint64_t *)(ap_bootstrap + AP_DATA_OFFSET);

	ap_data[AP_DATA_STATUS] = 0;
	ap_data[AP_DATA_ENTRY_POINT] = (uint64_t)rk_ap_entry;
	ap_data[AP_DATA_CR3_VALUE] = rk_mmu_main_cr3;

	void *new_stack = malloc(NEW_STACK_SIZE);

	rk_spinlock_acquire(&timer_lock);

	/* initialize processors */
	size_t cpu_id = 0;
	for (size_t i = 0; i < lapic_count; i++) {

		uint32_t id = lapics[i].table->id;
		if (id < 256) lapic2cpu[id] = i;

		rk_device_t *device = rk_cpu_register(i);
		rk_cpu_device_t *cpu = RK_CPU_DEVICE(device);

		cpu->arch_data[0] = &rk_tss;

		if (!cpu || !lapics[i].table->flags)
			continue;

		if (lapics[i].id == lapic_id) {

			cpu->active = true;
			cpu_id = i;
			main_cpu = device;
			continue;
		}

		ap_index = i;
		ap_initialized = false;

		ap_data[AP_DATA_STACK_POINTER] = (uint64_t)new_stack + NEW_STACK_SIZE;

		/* send init ipi */
		rk_lapic_outd(RK_LAPIC_REG_ERROR_STATUS, 0);
		uint32_t destination = RK_LAPIC_CMD_DESTINATION(id);

		rk_lapic_outd(RK_LAPIC_REG_ICR1, destination);
		rk_lapic_outd(
			RK_LAPIC_REG_ICR0,
			RK_LAPIC_CMD_DELIVERY_INIT |
			RK_LAPIC_CMD_NO_INIT_LEVEL_DEASSERT
		);
		wait_for_delivery();

		/* init deassert */
		rk_lapic_outd(RK_LAPIC_REG_ICR1, destination);
		rk_lapic_outd(
			RK_LAPIC_REG_ICR0,
			RK_LAPIC_CMD_DELIVERY_INIT |
			RK_LAPIC_CMD_INIT_LEVEL_DEASSERT
		);
		wait_for_delivery();
		wait_ms(10);

		if (!(ap_data[AP_DATA_STACK_POINTER] & 0xf))
			ap_data[AP_DATA_STACK_POINTER] -= 8;

		/* send first startup ipi */
		rk_lapic_outd(RK_LAPIC_REG_ERROR_STATUS, 0);
		rk_lapic_outd(RK_LAPIC_REG_ICR1, destination);
		rk_lapic_outd(RK_LAPIC_REG_ICR0, RK_LAPIC_CMD_DELIVERY_SIPI | 1);
		wait_for_delivery();

		wait_us(400);

		/* send second startup ipi */
		if (!ap_data[0]) {

			rk_log_warning("First SIPI for CPU %zu failed; Trying again...", i);

			rk_lapic_outd(RK_LAPIC_REG_ERROR_STATUS, 0);
			rk_lapic_outd(RK_LAPIC_REG_ICR1, destination);
			rk_lapic_outd(RK_LAPIC_REG_ICR0, RK_LAPIC_CMD_DELIVERY_SIPI | 1);
			wait_for_delivery();

			uint32_t count = 100;
			for (; count && !ap_data[0]; count--)
				wait_ms(10);

			if (!count) {

				rk_log_warning("Can't initialize CPU %zu", i);
				continue;
			}
		}

		/* wait for complete initialization */
		if (i < lapic_count-1) new_stack = malloc(NEW_STACK_SIZE);
		while (!ap_initialized) asm volatile("hlt");

		cpu->active = true;
	}
	rk_spinlock_release(&timer_lock);

	/* restore data in bootstrap area */
	memcpy(ap_bootstrap, old_buffer, rk_mmu_page_size);
	free(old_buffer);
	rk_mmu_unmap(ap_bootstrap, rk_mmu_page_size);

	/* configure and calibrate apic timer */
	calibrate_timer(cpu_id);

	init = true;
	rk_cpu_signal_all(RK_CPU_COMMAND_READY, 0);

	rk_log_debug("Initialized all CPUs");
}

/* check cpu is ourself */
extern bool rk_cpu_is_self(rk_device_t *device) {

	if (!init || !device) return false;

	uint32_t lapic_id = RK_LAPIC_ID(rk_lapic_ind(RK_LAPIC_REG_ID));
	return lapic_id == lapics[RK_CPU_DEVICE(device)->id].id;
}

/* send signal to cpu */
extern void rk_cpu_signal(
		rk_device_t *device,
		uint64_t command,
		uint64_t userdata
) {
	if (!init || !device) return;
	rk_cpu_device_t *cpu = RK_CPU_DEVICE(device);

	rk_spinlock_acquire(&cpu->signal.lock);

	cpu->signal.command = command;
	cpu->signal.userdata = userdata;
	cpu->signal.acknowledged = false;

	uint32_t destination = RK_LAPIC_CMD_DESTINATION(lapics[cpu->id].table->id);

	rk_lapic_outd(RK_LAPIC_REG_ICR1, destination);
	rk_lapic_outd(
		RK_LAPIC_REG_ICR0,
		RK_IDT_IPI_VECTOR |
		RK_LAPIC_CMD_DELIVERY_NORMAL
	);
	wait_for_delivery();

	while (!cpu->signal.acknowledged);
	rk_spinlock_release(&cpu->signal.lock);
}

/* output to ioapic register */
extern void rk_ioapic_outd(
		volatile uint32_t *base,
		uint32_t offset,
		uint32_t data
) {

	*(base + RK_IOAPIC_COMMAND) = offset;
	*(base + RK_IOAPIC_DATA) = data;
}

/* input from ioapic register */
extern uint32_t rk_ioapic_ind(
		volatile uint32_t *base,
		uint32_t offset
) {

	*(base + RK_IOAPIC_COMMAND) = offset;
	return *(base + RK_IOAPIC_DATA);
}

/* output to local apic register */
extern void rk_lapic_outd(
		uint32_t offset,
		uint32_t data
) {
	*(volatile uint32_t *)((volatile void *)lapic_address + offset) = data;
}

/* input from local apic register */
extern uint32_t rk_lapic_ind(uint32_t offset) {

	return *(volatile uint32_t *)((volatile void *)lapic_address + offset);
}

/* enable vector */
extern void rk_ioapic_enable(
		size_t vector,
		uint32_t destination
) {
	size_t j = 0;
	for (; j < ioapic_count; j++) {
		if (vector >= ioapics[j].base &&
		    vector <= ioapics[j].base + ioapics[j].count)
			break;
	}
	if (j >= ioapic_count) return;

	volatile uint32_t *ioapic = ioapics[j].address;

	uint32_t index = (uint32_t)(vector - ioapics[j].base);
	uint32_t offset = RK_IOAPIC_REG_TABLE + index * 2;

	uint32_t flags = rk_ioapic_ind(ioapic, offset);
	flags &= ~(RK_IOAPIC_IRQ_MASK);

	rk_ioapic_outd(ioapic, offset, flags);
	rk_ioapic_outd(ioapic, offset+1, RK_IOAPIC_IRQ_DESTINATION(destination));
}

/* disable vector */
extern void rk_ioapic_disable(size_t vector) {

	size_t j = 0;
	for (; j < ioapic_count; j++) {
		if (vector >= ioapics[j].base &&
		    vector <= ioapics[j].base + ioapics[j].count)
			break;
	}
	if (j >= ioapic_count) return;

	volatile uint32_t *ioapic = ioapics[j].address;

	uint32_t index = (uint32_t)(vector - ioapics[j].base);
	uint32_t offset = RK_IOAPIC_REG_TABLE + index * 2;

	uint32_t flags = rk_ioapic_ind(ioapic, offset);
	flags |= RK_IOAPIC_IRQ_MASK;
	rk_ioapic_outd(ioapic, offset, flags);
}

/* acknowledge interrupt */
extern void rk_lapic_acknowledge(void) {

	rk_lapic_outd(RK_LAPIC_REG_EOI, 0);
}

/* ap entry point */
extern void arch_enable_fpu(void);

extern __attribute__((used)) void rk_ap_entry(void) {

	arch_enable_fpu();

	rk_tss_t *tss = (rk_tss_t *)
		rk_mmu_map(NULL, NULL, sizeof(rk_tss_t), 0);
	memset(tss, 0, sizeof(rk_tss_t));

	rk_gdt_load(tss);
	rk_idt_load();

	rk_device_t *device = get_local_cpu();
	rk_cpu_device_t *cpu = RK_CPU_DEVICE(device);

	rk_spinlock_acquire(&cpu->global_lock);
	cpu->arch_data[0] = tss;

	rk_log_info("Initialized CPU %zu", ap_index);
	ap_initialized = true;

	/* configure lapic */
	rk_lapic_outd(RK_LAPIC_REG_SPURIOUS_INTERRUPT, 0x1ff);
	asm volatile("sti");

	calibrate_timer(cpu->id);

	/* wait until the main processor gives the go-ahead */
	while (!cpu->ready)
		asm volatile("hlt");

	asm volatile("cli");
	while (1) asm volatile("hlt");
}
