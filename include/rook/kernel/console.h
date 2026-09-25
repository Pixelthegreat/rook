#ifndef ROOK_KERNEL_CONSOLE_H
#define ROOK_KERNEL_CONSOLE_H

#include <stdarg.h>
#include <rook/core.h>

/*
 * Initialize console
 */
extern void rk_console_init(void);

/*
 * Print message to console
 */
extern void rk_console_prints(const char *message);

/*
 * Log message
 */
typedef enum rk_log_level {
	RK_LOG_LEVEL_DEBUG = 0,
	RK_LOG_LEVEL_INFO,
	RK_LOG_LEVEL_WARNING,
	RK_LOG_LEVEL_ERROR,
	RK_LOG_LEVEL_FATAL,

	RK_LOG_LEVEL_COUNT,
} rk_log_level_t;

extern void rk_logv(
		rk_log_level_t level,
		const char *path,
		int line,
		const char *module_name,
		const char *format,
		va_list args
);

extern void rk_log(
		rk_log_level_t level,
		const char *path,
		int line,
		const char *module_name,
		const char *format,
		...
);

#define rk_log_debug(...) rk_log(RK_LOG_LEVEL_DEBUG, __FILE__, __LINE__, RK_MODULE_NAME, __VA_ARGS__)
#define rk_log_info(...) rk_log(RK_LOG_LEVEL_INFO, __FILE__, __LINE__, RK_MODULE_NAME, __VA_ARGS__)
#define rk_log_warning(...) rk_log(RK_LOG_LEVEL_WARNING, __FILE__, __LINE__, RK_MODULE_NAME, __VA_ARGS__)
#define rk_log_error(...) rk_log(RK_LOG_LEVEL_ERROR, __FILE__, __LINE__, RK_MODULE_NAME, __VA_ARGS__)
#define rk_log_fatal(...) rk_log(RK_LOG_LEVEL_FATAL, __FILE__, __LINE__, RK_MODULE_NAME, __VA_ARGS__)

/*
 * Panic
 */
extern void rk_panic_long(
		void *context,
		const char *path,
		int line,
		const char *module_name,
		const char *format,
		...
);

#define rk_panic(context, ...) rk_panic_long(context, __FILE__, __LINE__, RK_MODULE_NAME, __VA_ARGS__)

extern void rk_print_panic_context(void *context); //arch//

#endif /* ROOK_KERNEL_CONSOLE_H */
