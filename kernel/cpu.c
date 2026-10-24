/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/mmu.h>
#include <rook/kernel/console.h>
#include <rook/kernel/cpu.h>

rk_device_class_t rk_device_class_cpu =
	RK_DEVICE_CLASS(cpu, sizeof(rk_cpu_device_t));

/* register cpu if not already registered */
extern rk_device_t *rk_cpu_register(size_t id) {

	rk_device_t *device = rk_cpu_get(id);
	if (device) return device;

	char name[32];
	snprintf(
		name, sizeof(name),
		"cpu%zu", id
	);

	device = rk_device_create(
			name,
			"CPU",
			&rk_device_class_cpu
	);
	RK_CPU_DEVICE(device)->id = id;

	rk_log_info("Registered CPU %zu", id);
	return device;
}

/* get cpu for id */
extern rk_device_t *rk_cpu_get(size_t id) {

	rk_device_t *device = rk_device_class_cpu.first;
	for (; device; device = device->class_next) {

		if (RK_CPU_DEVICE(device)->id == id)
			return device;
	}
	return NULL;
}

/* get next active cpu */
extern rk_device_t *rk_cpu_get_next_active(rk_device_t *device) {

	if (!device) device = rk_device_class_cpu.first;
	else device = device->class_next;

	while (device && !RK_CPU_DEVICE(device)->active)
		device = device->class_next;

	return device;
}

/* handle signal */
extern void rk_cpu_signaled(rk_device_t *device) {

	rk_cpu_device_t *cpu = RK_CPU_DEVICE(device);

	uint64_t command = cpu->signal.command;
	uint64_t userdata = cpu->signal.userdata;

	switch (command) {

		/* acknowledge test ping */
		case RK_CPU_COMMAND_PING:
			rk_log_debug("Hello from processor %zu! Ping: %llu", cpu->id, userdata);
			break;

		/* tlb shootdown */
		case RK_CPU_COMMAND_SHOOTDOWN:
			rk_mmu_invalidate((void *)userdata);
break;

		/* cpu is ready to be used */
		case RK_CPU_COMMAND_READY:
			cpu->ready = true;
			break;

		/* TODO: halt processor */
		case RK_CPU_COMMAND_HALT:
			break;
	}
	cpu->signal.acknowledged = true;
}

/* send signal to all cpus */
extern void rk_cpu_signal_all(
		uint64_t command,
		uint64_t userdata
) {

	rk_device_t *device = rk_cpu_get_next_active(NULL);
	while (device) {

		if (!rk_cpu_is_self(device))
			rk_cpu_signal(device, command, userdata);
		device = rk_cpu_get_next_active(device);
	}
}
