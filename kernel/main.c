#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/console.h>
#include <rook/kernel/meta.h>
#include <rook/kernel/module.h>

extern void main(void) {

	rk_boot_init();

	/* initialize builtin modules */
	rk_meta_t *meta = rk_meta_next(NULL, RK_META_TYPE_MODULE);
	for (; meta; meta = rk_meta_next(meta, RK_META_TYPE_MODULE)) {

		rk_module_t *module = (rk_module_t *)meta;

		if (module->type == RK_MODULE_TYPE_NORMAL)
			rk_module_init(module);
	}
	rk_console_init();
	rk_console_prints("\n");

	for (size_t i = 0; i < 8; i++) {

		char buffer[256];
		snprintf(
			buffer, sizeof(buffer),
			"  \e[3%zumHello, world!\e[39m   \e[9%zumHello, world!\e[39m\n",
			i, i
		);
		rk_console_prints(buffer);
	}
	rk_console_prints("\n");
	rk_panic(NULL, "Kernel panic!");
}
