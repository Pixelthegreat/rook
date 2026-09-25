#include <rook/core.h>
#include <rook/module.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/console.h>
#include <rook/kernel/meta.h>

extern void main(void) {

	rk_console_init();
	rk_console_prints("Hello, world!\nModules:\n");

	rk_meta_t *meta = rk_meta_next(NULL, RK_META_TYPE_MODULE);
	for (; meta; meta = rk_meta_next(meta, RK_META_TYPE_MODULE)) {

		rk_module_t *module = (rk_module_t *)meta;

		rk_console_printc('\t');
		rk_console_prints(module->name);
		rk_console_printc('\n');
	}
	rk_console_prints("-- End --\n");
}
