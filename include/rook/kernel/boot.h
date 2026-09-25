#ifndef ROOK_KERNEL_BOOT_H
#define ROOK_KERNEL_BOOT_H

#include <rook/core.h>
#include <rook/meta.h>

/*
 * Boot protocol info
 */
typedef struct rk_boot_info {
	struct {
		char name[32];
		uint8_t version[3];
	} bootloader;

	char cmdline[128];
} rk_boot_info_t;

extern rk_boot_info_t rk_boot_info;

/*
 * Boot protocol record
 */
typedef struct rk_meta_boot_protocol {
	rk_meta_t base;
	char name[32];
	void (*init)(void);
} rk_meta_boot_protocol_t;

#endif /* ROOK_KERNEL_BOOT_H */
