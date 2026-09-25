#include <rook/core.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/console.h>
#include <rook/kernel/meta.h>
#include <rook/kernel/module.h>

extern void main(void) {

	/* initialize builtin modules */
	rk_meta_t *meta = rk_meta_next(NULL, RK_META_TYPE_MODULE);
	for (; meta; meta = rk_meta_next(meta, RK_META_TYPE_MODULE)) {

		rk_module_t *module = (rk_module_t *)meta;

		if (module->type == RK_MODULE_TYPE_NORMAL)
			rk_module_init(module);
	}
	rk_console_init();

	rk_log_debug("Debug message");
	rk_log_info("Info message");
	rk_log_warning("Warning message");
	rk_log_error("Error message");
	rk_log_fatal("Fatal message");
}
