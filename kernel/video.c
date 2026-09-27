#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/video.h>

rk_device_class_t rk_device_class_video =
	RK_DEVICE_CLASS(video, sizeof(rk_video_device_t));

/* configure mode for output */
extern bool rk_video_device_configure_output(
		rk_device_t *device,
		rk_video_output_t *output,
		rk_video_mode_t *mode
) {
	rk_video_device_t *video = RK_VIDEO_DEVICE(device);

	if (!video->configure_output)
		return false;
	return video->configure_output(video, output, mode);
}

/* get current mode for output */
extern void rk_video_device_get_output_mode(
		rk_device_t *device,
		rk_video_output_t *output,
		rk_video_mode_t *mode
) {
	rk_video_device_t *video = RK_VIDEO_DEVICE(device);

	memset(mode, 0, sizeof(rk_video_mode_t));

	if (!video->get_output_mode)
		return;
	video->get_output_mode(video, output, mode);
}

/* get physical framebuffer address for output */
extern void *rk_video_device_get_framebuffer_address(
		rk_device_t *device,
		rk_video_output_t *output
) {
	rk_video_device_t *video = RK_VIDEO_DEVICE(device);

	if (!video->get_framebuffer_address)
		return NULL;
	return video->get_framebuffer_address(video, output);
}

/* get framebuffer attributes for output */
extern void rk_video_device_get_framebuffer_attributes(
		rk_device_t *device,
		rk_video_output_t *output,
		rk_video_framebuffer_attributes_t *attributes
) {
	rk_video_device_t *video = RK_VIDEO_DEVICE(device);

	memset(
		attributes, 0,
		sizeof(rk_video_framebuffer_attributes_t)
	);
	if (!video->get_framebuffer_attributes)
		return;
	video->get_framebuffer_attributes(
			video, output, attributes
	);
}

/* get mode list */
extern void rk_video_device_get_modes(
		rk_device_t *device,
		rk_video_mode_t **p_modes,
		size_t *p_mode_count
) {
	rk_video_device_t *video = RK_VIDEO_DEVICE(device);

	*p_modes = video->modes;
	*p_mode_count = video->mode_count;
}

/* get output list */
extern void rk_video_device_get_outputs(
		rk_device_t *device,
		rk_video_output_t **p_outputs,
		size_t *p_output_count
) {
	rk_video_device_t *video = RK_VIDEO_DEVICE(device);

	*p_outputs = video->outputs;
	*p_output_count = video->output_count;
}

/* pick appropriate video mode for output */
extern rk_video_mode_t *rk_video_pick_mode_for_output(
		rk_video_output_t *output,
		rk_video_mode_t *modes,
		size_t mode_count
) {
	if (!mode_count)
		return NULL;

	rk_video_mode_t *smallest = NULL;
	rk_video_mode_t *closest = NULL;

	long size = (long)(output->width * output->height);
	long diff = 0;

	/* analyze modes */
	rk_video_mode_t *mode = modes;
	for (size_t i = 0; i < mode_count; i++, mode++) {

		long aux_size = (long)(mode->width * mode->height);

		long aux_diff = size - aux_size;
		aux_diff = RK_ABS(aux_diff);

		if (!closest || aux_diff < diff) {

			closest = mode;
			diff = aux_diff;
		}
		if (!smallest ||
		    aux_size < (long)(smallest->width * smallest->height))
			smallest = mode;
	}

	/* select mode */
	mode = closest;

	if (mode->width != output->width ||
	    mode->height != output->height) {

		mode = smallest;
		if (mode->width > output->width ||
		    mode->height > output->height)
			return NULL;
	}
	return mode;
}
