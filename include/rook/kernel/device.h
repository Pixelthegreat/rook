#ifndef ROOK_KERNEL_DEVICE_H
#define ROOK_KERNEL_DEVICE_H

#include <rook/core.h>

struct rk_device;

/*
 * Device class
 */
typedef struct rk_device_class {
	char name[32];
	size_t size;
	struct rk_device *first, *last;
} rk_device_class_t;

#define RK_DEVICE_CLASS(p_name, p_size) {.name = #p_name, .size = (p_size)}

extern rk_device_class_t rk_device_class_bus;

/*
 * Device
 */
typedef struct rk_device {
	char name[32];
	char description[32];
	rk_device_class_t *class;
	struct rk_device *bus;
	struct rk_device *class_previous, *class_next;
	struct rk_device *bus_previous, *bus_next;
	int refcnt;
} rk_device_t;

#define RK_DEVICE(p) ((rk_device_t *)(p))

/*
 * Bus device
 */
typedef struct rk_bus_device {
	rk_device_t base;
	rk_device_t *first, *last;
} rk_bus_device_t;

#define RK_BUS_DEVICE(p) ((rk_bus_device_t *)(p))

/*
 * Create device
 */
extern rk_device_t *rk_device_create(
		const char *name,
		const char *description,
		rk_device_class_t *class
);

/*
 * Register device
 */
extern void rk_device_register(
		rk_device_t *device,
		const char *name,
		const char *description,
		rk_device_class_t *class
);

/*
 * Reference device
 */
extern rk_device_t *rk_device_reference(rk_device_t *device);

/*
 * Unregister device
 */
extern void rk_device_unregister(rk_device_t *device);

/*
 * Destroy device
 */
extern void rk_device_destroy(rk_device_t *device);

/*
 * Add device to bus
 */
extern void rk_bus_device_add(rk_device_t *bus, rk_device_t *device);

/*
 * Remove device from bus
 */
extern void rk_bus_device_remove(rk_device_t *bus, rk_device_t *device);

#endif /* ROOK_KERNEL_DEVICE_H */
