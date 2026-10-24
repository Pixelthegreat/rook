#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/lock.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/device.h>
#include <rook/kernel/tty.h>
#include <rook/kernel/console.h>

static rk_device_t *devices[RK_BOOT_CMDLINE_MAX_CONSOLES];
static size_t device_count = 0;

static char buffer_a[1024];
static char buffer_b[1024];

static char backlog[0x8000] = "====\n";
static size_t backlog_start = 0, backlog_end = 5;

static rk_spinlock_t lock;

/* add to backlog */
static void add_backlog(const char *message) {

	for (; *message; message++) {

		if (backlog_start == backlog_end)
			backlog_start++;
		size_t i = backlog_end++;

		backlog_start %= sizeof(backlog);
		backlog_end %= sizeof(backlog);

		backlog[i] = *message;
	}
}

/* initialize console */
extern void rk_console_init(void) {

	for (size_t i = 0; i < rk_boot_info.cmdline.console_count; i++) {

		const char *name = rk_boot_info.cmdline.consoles[i];

		rk_device_t *device = rk_device_lookup(
				&rk_device_class_tty, name
		);
		if (!device) {

			rk_log_error("Can't find TTY device '%s'", name);
			continue;
		}
		devices[device_count++] = device;
	}
	if (!device_count) devices[0] = rk_device_class_tty.first;

	rk_console_write(
		backlog,
		backlog_end
	);
}

/* write data to console */
extern void rk_console_write(const char *buffer, size_t count) {

	for (size_t i = 0; i < device_count; i++)
		rk_tty_device_write(
			devices[i],
			buffer,
			count
		);
}

/* print message to console */
extern void rk_console_prints(const char *message) {

	rk_console_write(
		message,
		strlen(message)
	);
}

/* log message to console with argument list */
extern void rk_logv(
		rk_log_level_t level,
		const char *path,
		int line,
		const char *module_name,
		const char *format,
		va_list args
) {
	rk_spinlock_acquire(&lock);

	switch (level) {
		case RK_LOG_LEVEL_DEBUG:
			snprintf(
				buffer_a, sizeof(buffer_a),
				"\e[90m[%s:%s:%d] ",
				module_name,
				path,
				line
			);
			break;
		case RK_LOG_LEVEL_INFO:
			snprintf(
				buffer_a, sizeof(buffer_a),
				"[%s] ",
				module_name
			);
			break;
		case RK_LOG_LEVEL_WARNING:
			snprintf(
				buffer_a, sizeof(buffer_a),
				"\e[33m[%s] ",
				module_name
			);
			break;
		case RK_LOG_LEVEL_ERROR:
			snprintf(
				buffer_a, sizeof(buffer_a),
				"\e[31m[%s:%s:%d] ",
				module_name,
				path,
				line
			);
			break;
		case RK_LOG_LEVEL_FATAL:
			snprintf(
				buffer_a, sizeof(buffer_a),
				"\e[41m\e[91m[%s:%s:%d] ",
				module_name,
				path,
				line
			);
			break;
	}

	/* print message */
	vsnprintf(buffer_b, sizeof(buffer_b), format, args);

	rk_console_prints(buffer_a);
	rk_console_prints(buffer_b);
	rk_console_prints("\e[39m\e[49m\n");

	add_backlog(buffer_a);
	add_backlog(buffer_b);
	add_backlog("\e[39m\e[49m\n");

	rk_spinlock_release(&lock);
}

/* log message */
extern void rk_log(
		rk_log_level_t level,
		const char *path,
		int line,
		const char *module_name,
		const char *format,
		...
) {
	va_list args;
	va_start(args, format);
	rk_logv(level, path, line, module_name, format, args);
	va_end(args);
}

/* panic */
extern void rk_panic_long(
		void *context,
		const char *path,
		int line,
		const char *module_name,
		const char *format,
		...
) {
	va_list args;
	va_start(args, format);

	rk_logv(
		RK_LOG_LEVEL_FATAL,
		path, line,
		module_name, format,
		args
	);
	va_end(args);

	if (context) rk_print_panic_context(context);

	asm volatile("cli");
	while (1) asm volatile("hlt"); /* TODO: halt */
}
