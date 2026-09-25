#ifndef ROOK_MODULE_H
#define ROOK_MODULE_H

#include <rook/core.h>
#include <rook/kernel/meta.h>

/*
 * Module record
 */
typedef enum rk_module_type {
	RK_MODULE_TYPE_NORMAL = 0,

	RK_MODULE_TYPE_COUNT,
} rk_module_type_t;

typedef struct rk_module {
	rk_meta_t base;
	char name[32];
	rk_module_type_t type;
	void (*init)(void);
	void (*fini)(void);
} __attribute__((packed)) rk_module_t;

#define RK_MODULE(p_name, p_type, p_init, p_fini) static RK_META rk_module_t _module = {\
	.base = {\
		.type = RK_META_TYPE_MODULE,\
		.size = sizeof(rk_module_t),\
	},\
	.name = (p_name),\
	.type = (p_type),\
	.init = (p_init),\
	.fini = (p_fini),\
}

/*
 * Initialize module
 */
extern void rk_module_init(rk_module_t *module);

/*
 * Deinitialize module
 */
extern void rk_module_fini(rk_module_t *module);

#endif /* ROOK_MODULE_H */
