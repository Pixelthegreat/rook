#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/mmu.h>
#include <rook/kernel/meta.h>
#include <rook/kernel/module.h>
#include <rook/kernel/console.h>

extern void main(void) {

	rk_boot_init();
	rk_mmu_init();

	/* initialize builtin modules */
	rk_meta_t *meta = rk_meta_next(NULL, RK_META_TYPE_MODULE);
	for (; meta; meta = rk_meta_next(meta, RK_META_TYPE_MODULE)) {

		rk_module_t *module = (rk_module_t *)meta;

		if (module->type == RK_MODULE_TYPE_NORMAL)
			rk_module_init(module);
	}
	rk_console_init();
}
