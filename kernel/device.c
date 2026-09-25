#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/device.h>

/* register device */
extern void rk_device_register(
		rk_device_t *device,
		const char *name,
		rk_device_class_t *class
) {
	rk_strncpy(device->name, sizeof(device->name), name);
	device->name[sizeof(device->name)-1] = 0;

	device->class = class;

	device->class_previous = class->last;
	if (!class->first) class->first = device;
	if (class->last) class->last->class_next = device;
	class->last = device;

	device->refcnt = 1;
}

/* reference device */
extern rk_device_t *rk_device_reference(rk_device_t *device) {

	device->refcnt++;
	return device;
}

/* unregister device */
extern void rk_device_unregister(rk_device_t *device) {

	/* TODO: rk_device_bus_remove(); */

	if (device->class->first == device)
		device->class->first = device->class_next;
	if (device->class->last == device)
		device->class->last = device->class_previous;
	if (device->class_previous)
		device->class_previous->class_next = device->class_next;
	if (device->class_next)
		device->class_next->class_previous = device->class_previous;

	device->class = NULL;
}
