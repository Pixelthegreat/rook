#ifndef ROOK_KERNEL_VIDEO_H
#define ROOK_KERNEL_VIDEO_H

#include <rook/core.h>
#include <rook/common/video.h>
#include <rook/kernel/device.h>

/*
 * Video device
 */
typedef struct rk_video_device {
	rk_device_t base;

	rk_video_mode_t *modes;
	size_t mode_count;

	rk_video_output_t *outputs;
	size_t output_count;

	/*
	 * Configure mode for output
	 */
	bool (*configure_output)(
			struct rk_video_device *device,
			rk_video_output_t *output,
			rk_video_mode_t *mode
	);

	/*
	 * Get current mode for output
	 */
	void (*get_output_mode)(
			struct rk_video_device *device,
			rk_video_output_t *output,
			rk_video_mode_t *mode
	);

	/*
	 * Get physical framebuffer address for output
	 */
	void *(*get_framebuffer_address)(
			struct rk_video_device *device,
			rk_video_output_t *output
	);

	/*
	 * Get framebuffer attributes for output
	 */
	void (*get_framebuffer_attributes)(
			struct rk_video_device *device,
			rk_video_output_t *output,
			rk_video_framebuffer_attributes_t *attributes
	);
} rk_video_device_t;

#define RK_VIDEO_DEVICE(p) ((rk_video_device_t *)(p))

extern rk_device_class_t rk_device_class_video;

/*
 * Configure mode for output
 *
 * Returns true if the configuration was successful,
 * false otherwise.
 */
extern bool rk_video_device_configure_output(
		rk_device_t *device,
		rk_video_output_t *output,
		rk_video_mode_t *mode
);

/*
 * Get current mode for output
 *
 * Copies the mode into 'mode'.
 */
extern void rk_video_device_get_output_mode(
		rk_device_t *device,
		rk_video_output_t *output,
		rk_video_mode_t *mode
);

/*
 * Get physical framebuffer address for output
 */
extern void *rk_video_device_get_framebuffer_address(
		rk_device_t *device,
		rk_video_output_t *output
);

/*
 * Get framebuffer attributes for output
 *
 * Copies the attributes into 'attributes'.
 */
extern void rk_video_device_get_framebuffer_attributes(
		rk_device_t *device,
		rk_video_output_t *output,
		rk_video_framebuffer_attributes_t *attributes
);

/*
 * Get mode list
 */
extern void rk_video_device_get_modes(
		rk_device_t *device,
		rk_video_mode_t **p_modes,
		size_t *p_mode_count
);

/*
 * Get output list
 */
extern void rk_video_device_get_outputs(
		rk_device_t *device,
		rk_video_output_t **p_outputs,
		size_t *p_output_count
);

/*
 * Pick appropriate video mode for output
 */
extern rk_video_mode_t *rk_video_pick_mode_for_output(
		rk_video_output_t *output,
		rk_video_mode_t *modes,
		size_t mode_count
);

#endif /* ROOK_KERNEL_VIDEO_H */
