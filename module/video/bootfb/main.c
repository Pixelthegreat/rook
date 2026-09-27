#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/console.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/module.h>
#include <rook/kernel/video.h>

/* module info */
static void module_init(void);
static void module_fini(void);

RK_MODULE(
	"bootfb",
	RK_MODULE_TYPE_NORMAL,
	module_init,
	module_fini,
	RK_MODULE_PRIORITY_MIDDLE
);

/* device info */
static rk_video_mode_t mode;
static rk_video_output_t output;

static bool video_configure_output(
		rk_video_device_t *_device,
		rk_video_output_t *_output,
		rk_video_mode_t *_mode
);
static void video_get_output_mode(
		rk_video_device_t *_device,
		rk_video_output_t *_output,
		rk_video_mode_t *_mode
);
static void *video_get_framebuffer_address(
		rk_video_device_t *_device,
		rk_video_output_t *_output
);
static void video_get_framebuffer_attributes(
		rk_video_device_t *_device,
		rk_video_output_t *_output,
		rk_video_framebuffer_attributes_t *attributes
);

static rk_video_device_t video = {
	.modes = &mode,
	.mode_count = 1,

	.outputs = &output,
	.output_count = 1,

	.configure_output = video_configure_output,
	.get_output_mode = video_get_output_mode,
	.get_framebuffer_address = video_get_framebuffer_address,
	.get_framebuffer_attributes = video_get_framebuffer_attributes,
};
static rk_device_t *device = RK_DEVICE(&video);

/* configure mode for output */
static bool video_configure_output(
		rk_video_device_t *_device,
		rk_video_output_t *_output,
		rk_video_mode_t *_mode
) {
	if (!!memcmp(_mode, &mode, sizeof(rk_video_mode_t)) ||
	    !!memcmp(_output, &output, sizeof(rk_video_output_t)))
		return false;

	return true;
}

/* get current mode for output */
static void video_get_output_mode(
		rk_video_device_t *_device,
		rk_video_output_t *_output,
		rk_video_mode_t *_mode
) {
	if (_output != &output)
		return;
	memcpy(_mode, &mode, sizeof(rk_video_mode_t));
}

/* get physical framebuffer address */
static void *video_get_framebuffer_address(
		rk_video_device_t *_device,
		rk_video_output_t *_output
) {
	if (_output != &output)
		return NULL;
	return rk_boot_info.framebuffer.physical;
}

/* get framebuffer attributes for output */
static void video_get_framebuffer_attributes(
		rk_video_device_t *_device,
		rk_video_output_t *_output,
		rk_video_framebuffer_attributes_t *attributes
) {
	if (_output != &output)
		return;
	memcpy(
		attributes,
		&rk_boot_info.framebuffer.attributes,
		sizeof(rk_video_framebuffer_attributes_t)
	);
}

/* initialize module */
static void module_init(void) {

	if (!rk_boot_info.framebuffer.virtual) {

		rk_log_error("No framebuffer available");
		return;
	}

	/* register device */
	output.width = rk_boot_info.framebuffer.attributes.width;
	output.height = rk_boot_info.framebuffer.attributes.height;

	mode.width = rk_boot_info.framebuffer.attributes.width;
	mode.height = rk_boot_info.framebuffer.attributes.height;
	mode.depth = rk_boot_info.framebuffer.attributes.depth_bits;
	mode.layout = rk_boot_info.framebuffer.attributes.layout;

	rk_device_register(
			device,
			"vid0",
			"Bootloader Framebuffer",
			&rk_device_class_video
	);
}

/* deinitialize module */
static void module_fini(void) {
}
