#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/device.h>
#include <rook/kernel/tty.h>
#include <rook/kernel/console.h>

static rk_device_t *device;

static char buffer_a[1024];
static char buffer_b[1024];

/* initialize console */
extern void rk_console_init(void) {

	device = rk_device_class_tty.first;

	rk_log_info("Beginning of log");
}

/* print message to console */
extern void rk_console_prints(const char *message) {

	if (!device) return;

	rk_tty_device_write(
			device,
			message,
			strlen(message)
	);
}

/* log message to console */
extern void rk_log(
		rk_log_level_t level,
		const char *path,
		int line,
		const char *module_name,
		const char *format,
		...
) {
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
	va_list args;
	va_start(args, format);
	vsnprintf(buffer_b, sizeof(buffer_b), format, args);
	va_end(args);

	rk_console_prints(buffer_a);
	rk_console_prints(buffer_b);
	rk_console_prints("\e[39m\e[49m\n");
}
