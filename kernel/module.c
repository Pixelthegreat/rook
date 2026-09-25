#include <rook/core.h>
#include <rook/kernel/module.h>

/* initialize module */
extern void rk_module_init(rk_module_t *module) {

	if (module->init)
		module->init();
	/* TODO: log */
}

/* deinitialize module */
extern void rk_module_fini(rk_module_t *module) {

	if (module->fini)
		module->fini();
}
