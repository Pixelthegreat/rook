#ifndef ROOK_COMMON_VIDEO_H
#define ROOK_COMMON_VIDEO_H

#include <rook/core.h>

/*
 * Framebuffer memory model
 */
#define RK_VIDEO_MODEL_RGB 1

/*
 * Common data layouts
 */
#define RK_VIDEO_LAYOUT_UNKNOWN 0

#define RK_VIDEO_LAYOUT_RGB24 1
#define RK_VIDEO_LAYOUT_BGR24 2
#define RK_VIDEO_LAYOUT_RGBX32 3
#define RK_VIDEO_LAYOUT_BGRX32 4

/*
 * Framebuffer description
 */
typedef struct rk_video_framebuffer_attributes {
	size_t width;
	size_t height;
	size_t stride;
	size_t depth_bits;
	size_t depth_bytes;
	uint64_t model;
	uint64_t layout; /* may be UNKNOWN */
	union {
		/*
		 * RGB model information
		 */
		struct {
			struct {
				size_t size;
				size_t position;
			} rmask, gmask, bmask;
		} rgb;
	};
} rk_video_framebuffer_attributes_t;

/*
 * Video mode description
 */
typedef struct rk_video_mode {
	size_t width, height;
	size_t depth;
	uint64_t layout;
} rk_video_mode_t;

/*
 * Output description
 */
typedef struct rk_video_output {
	size_t id;
	size_t width, height;
} rk_video_output_t;

#endif /* ROOK_COMMON_VIDEO_H */
