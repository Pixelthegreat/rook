#ifndef ROOK_MODULE_H
#define ROOK_MODULE_H

#include <rook/core.h>
#include <rook/meta.h>

/*
 * Module record
 */
typedef struct rk_module {
	rk_meta_t base;
	char name[32];
	void (*init)(void);
	void (*fini)(void);
} __attribute__((packed)) rk_module_t;

#define RK_MODULE(p_name, p_init, p_fini) static RK_META rk_module_t _module = {\
	.base = {\
		.type = RK_META_TYPE_MODULE,\
		.size = sizeof(rk_module_t),\
	},\
	.name = #p_name,\
	.init = p_init,\
	.fini = p_fini,\
}

#endif /* ROOK_MODULE_H */
