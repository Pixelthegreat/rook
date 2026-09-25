#include <rook/core.h>
#include <rook/kernel/meta.h>

extern rk_meta_t _rk_meta_start;
extern rk_meta_t _rk_meta_end;

/* get next metadata entry for type */
extern rk_meta_t *rk_meta_next(rk_meta_t *entry, rk_meta_type_t type) {

	void *data = (void *)entry + entry->size;
	if (!data) data = (void *)&_rk_meta_start;

	void *end = (void *)&_rk_meta_end;

	while (data < end && ((rk_meta_t *)data)->type != type)
		data += ((rk_meta_t *)data)->size;

	if (data >= end)
		return NULL;
	return (rk_meta_t *)data;
}
