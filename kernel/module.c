#include <rook/core.h>
#include <rook/kernel/console.h>
#include <rook/kernel/module.h>

/* initialize module */
extern void rk_module_init(rk_module_t *module) {

	if (module->init)
		module->init();
	rk_log_info("Initialized module '%s'", module->name);
}

/* deinitialize module */
extern void rk_module_fini(rk_module_t *module) {

	if (module->fini)
		module->fini();
	rk_log_info("Deinitialized module '%s'", module->name);
}
