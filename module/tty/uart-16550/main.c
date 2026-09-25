#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/module.h>
#include <rook/kernel/device.h>
#include <rook/kernel/tty.h>
#include <rook/kernel/arch/x86_64/isa.h>

#define THR 0
#define RBR 0
#define DLL 0
#define IER 1
#define DLH 1
#define IIR 2
#define FCR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6
#define SR 7
#define PORT 0x3f8

static rk_tty_device_t tty;
static rk_device_t *device = RK_DEVICE(&tty);

/* module info */
static void module_init(void);
static void module_fini(void);

RK_MODULE("uart-16550", RK_MODULE_TYPE_NORMAL, module_init, module_fini);

/* write to tty */
static void putc_tty(char c) {

	for (int i = 0; !(isa_inb(PORT+LSR) & 0x20) && i < 65535; i++);
	isa_outb(PORT+THR, (uint8_t)c);

	if (c == '\n') putc_tty('\r');
}

static size_t tty_write(rk_tty_device_t *_, const char *buffer, size_t count) {

	for (size_t i = 0; i < count; i++)
		putc_tty(buffer[i]);
}

/* initialize module */
static void module_init(void) {

	rk_device_register(device, "16550 UART", &rk_device_class_tty);

	tty.write = tty_write;

	isa_outb(PORT+FCR, 0);
	isa_outb(PORT+LCR, 0x80);
	isa_outb(PORT+DLL, 12);
	isa_outb(PORT+DLH, 0);
	isa_outb(PORT+LCR, 0x03);
	isa_outb(PORT+MCR, 0);
	isa_outb(PORT+IER, 1);

	isa_inb(PORT+LSR);
}

/* deinitialize module */
static void module_fini(void) {

	rk_device_unregister(device);
}
