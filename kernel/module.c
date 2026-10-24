/*
 * Copyright 2026, Elliot Kohlmyer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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

/* initialize builtin modules */
extern void rk_module_init_builtins(
		int priority_start,
		int priority_end
) {
	rk_meta_t *meta = rk_meta_next(NULL, RK_META_TYPE_MODULE);
	for (; meta; meta = rk_meta_next(meta, RK_META_TYPE_MODULE)) {

		rk_module_t *module = (rk_module_t *)meta;

		if (module->priority < priority_start ||
		    module->priority >= priority_end)
			continue;

		rk_module_init(module);
	}
}
