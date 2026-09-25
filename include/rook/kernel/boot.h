#ifndef ROOK_KERNEL_BOOT_H
#define ROOK_KERNEL_BOOT_H

#include <rook/core.h>
#include <rook/common/video.h>
#include <rook/kernel/meta.h>

/*
 * Boot protocol info
 */
typedef struct rk_boot_info {
	struct {
		char name[32];
		uint8_t version[3];
	} bootloader;

	struct {
		char value[128];
		char console[32];
	} cmdline;

	struct {
		void *physical;
		void *virtual;
		size_t size;
		rk_video_framebuffer_attributes_t attributes;
	} framebuffer;
} rk_boot_info_t;

extern rk_boot_info_t rk_boot_info;

/*
 * Boot protocol record
 */
typedef struct rk_meta_boot_protocol {
	rk_meta_t base;
	char name[32];
	void (*init)(void);
} __attribute__((packed)) rk_boot_protocol_t;

#define RK_BOOT_PROTOCOL(p) ((rk_boot_protocol_t *)(p))

/*
 * Analyze boot info
 */
extern void rk_boot_init(void);

#endif /* ROOK_KERNEL_BOOT_H */
