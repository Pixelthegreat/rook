#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/console.h>
#include <rook/kernel/boot.h>

rk_boot_info_t rk_boot_info = {
	.cmdline = {
		.console = "tty0",
	},
};

/* initialize boot protocol info */
extern void rk_boot_init(void) {

	rk_meta_t *meta = rk_meta_next(NULL, RK_META_TYPE_BOOT_PROTOCOL);
	rk_boot_protocol_t *protocol = RK_BOOT_PROTOCOL(meta);

	if (!protocol || !protocol->init)
		rk_panic(NULL, "No supported boot protocol");

	protocol->init();

	/* parse command line */
	const char *cmdline = rk_boot_info.cmdline.value;
	while (*cmdline) {

		char key[128], value[128];

		memset(key, 0, sizeof(key));
		memset(value, 0, sizeof(value));

		size_t key_size = 0, value_size = 0;

		while (*cmdline && !strchr(" =", *cmdline)) {
			if (key_size < sizeof(key)-1)
				key[key_size++] = *cmdline;
			cmdline++;
		}
		key[key_size] = 0;

		char ch = *cmdline;
		if (ch) cmdline++;

		if (ch == '=') {

			while (*cmdline && *cmdline != ' ') {
				if (value_size < sizeof(value)-1)
					value[value_size++] = *cmdline;
				cmdline++;
			}
			if (*cmdline) cmdline++;
		}
		value[value_size] = 0;

		/* console device */
		if (!strcmp(key, "console"))
			rk_strncpy(
				rk_boot_info.cmdline.console,
				sizeof(rk_boot_info.cmdline.console),
				value
			);
	}

	/* print info */
	rk_log_debug(
		"Command line: '%s'",
		rk_boot_info.cmdline.value
	);

	if (rk_boot_info.framebuffer.virtual)
		rk_log_debug(
			"Framebuffer: width=%zu, height=%zu, stride=%zu, depth=%zu, layout=%llu",
			rk_boot_info.framebuffer.attributes.width,
			rk_boot_info.framebuffer.attributes.height,
			rk_boot_info.framebuffer.attributes.stride,
			rk_boot_info.framebuffer.attributes.depth_bits,
			rk_boot_info.framebuffer.attributes.layout
		);
}
