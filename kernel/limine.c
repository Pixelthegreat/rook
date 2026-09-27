#include <limine.h>
#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/boot.h>

#define MAX_ENTRIES 256
static rk_boot_memory_map_t memory_map[MAX_ENTRIES];

/* protocol metadata */
static void init(void);

RK_META() rk_boot_protocol_t protocol = {
	.base.type = RK_META_TYPE_BOOT_PROTOCOL,
	.base.size = sizeof(protocol),
	.name = "Limine 3.x",
	.init = init,
};

/* limine requests */
#define SECTION(name) __attribute__((used, section(#name))) static volatile

#define REQUEST SECTION(.limine_requests)
#define REQUEST_START SECTION(.limine_requests_start)
#define REQUEST_END SECTION(.limine_requests_end)

REQUEST LIMINE_BASE_REVISION(3);

REQUEST struct limine_bootloader_info_request bootloader_info_request = {
	.id = LIMINE_BOOTLOADER_INFO_REQUEST,
	.revision = 0,
};
REQUEST struct limine_executable_cmdline_request executable_cmdline_request = {
	.id = LIMINE_EXECUTABLE_CMDLINE_REQUEST,
	.revision = 0,
};
REQUEST struct limine_stack_size_request stack_size_request = {
	.id = LIMINE_STACK_SIZE_REQUEST,
	.revision = 0,
	.stack_size = 0x8000,
};
REQUEST struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0,
};
REQUEST struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0,
};
REQUEST struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0,
};
REQUEST struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
};
REQUEST struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0,
};

#if RK_CONFIG_ARCH == x86_64
REQUEST struct limine_paging_mode_request paging_mode_request = {
	.id = LIMINE_PAGING_MODE_REQUEST,
	.revision = 1,
	.mode = LIMINE_PAGING_MODE_X86_64_4LVL,
	.min_mode = LIMINE_PAGING_MODE_X86_64_4LVL,
	.max_mode = LIMINE_PAGING_MODE_X86_64_4LVL,
};
#endif

REQUEST_START LIMINE_REQUESTS_START_MARKER;
REQUEST_END LIMINE_REQUESTS_END_MARKER;

/* initialize boot protocol */
static void init(void) {

	/* command line */
	if (executable_cmdline_request.response)
		rk_strncpy(
			rk_boot_info.cmdline.value,
			sizeof(rk_boot_info.cmdline.value),
			executable_cmdline_request.response->cmdline
		);

	/* framebuffer */
	if (framebuffer_request.response &&
	    framebuffer_request.response->framebuffer_count) {

		struct limine_framebuffer *framebuffer =
			framebuffer_request.response->framebuffers[0];

		rk_video_framebuffer_attributes_t *attributes =
				&rk_boot_info.framebuffer.attributes;

		rk_boot_info.framebuffer.virtual = framebuffer->address;
		rk_boot_info.framebuffer.size =
			(size_t)framebuffer->pitch *
			(size_t)framebuffer->height;

		attributes->width = (size_t)framebuffer->width;
		attributes->height = (size_t)framebuffer->height;
		attributes->stride = (size_t)framebuffer->pitch;
		attributes->depth_bits = (size_t)framebuffer->bpp;
		attributes->depth_bytes = attributes->depth_bits >> 3;
		attributes->model = RK_VIDEO_MODEL_RGB;

		attributes->rgb.rmask.size = (size_t)framebuffer->red_mask_size;
		attributes->rgb.rmask.position = (size_t)framebuffer->red_mask_shift;

		attributes->rgb.gmask.size = (size_t)framebuffer->green_mask_size;
		attributes->rgb.gmask.position = (size_t)framebuffer->green_mask_shift;

		attributes->rgb.bmask.size = (size_t)framebuffer->blue_mask_size;
		attributes->rgb.bmask.position = (size_t)framebuffer->blue_mask_shift;

		/* try and determine layout */
		if (framebuffer->red_mask_size == 8 &&
		    framebuffer->red_mask_shift == 0 &&
		    framebuffer->green_mask_size == 8 &&
		    framebuffer->green_mask_shift == 8 &&
		    framebuffer->blue_mask_size == 8 &&
		    framebuffer->blue_mask_shift == 16) {

			if (attributes->depth_bytes == 4)
				attributes->layout = RK_VIDEO_LAYOUT_RGBX32;
			else if (attributes->depth_bytes == 3)
				attributes->layout = RK_VIDEO_LAYOUT_RGB24;
		}
		else if (framebuffer->red_mask_size == 8 &&
			 framebuffer->red_mask_shift == 16 &&
			 framebuffer->green_mask_size == 8 &&
			 framebuffer->green_mask_shift == 8 &&
			 framebuffer->blue_mask_size == 8 &&
			 framebuffer->blue_mask_shift == 0) {

			if (attributes->depth_bytes == 4)
				attributes->layout = RK_VIDEO_LAYOUT_BGRX32;
			else if (attributes->depth_bytes == 3)
				attributes->layout = RK_VIDEO_LAYOUT_BGR24;
		}
	}

	/* higher-half direct map */
	if (hhdm_request.response)
		rk_boot_info.hhdm.offset = hhdm_request.response->offset;

	/* acpi rsdp */
	if (rsdp_request.response)
		rk_boot_info.acpi.rsdp = (uint64_t)rsdp_request.response->address;

	/* memory map */
	if (memmap_request.response) {

		rk_boot_info.memory_map.entries = memory_map;
		for (size_t i = 0; i < memmap_request.response->entry_count; i++) {

			if (i >= MAX_ENTRIES) break;

			struct limine_memmap_entry *entry =
				memmap_request.response->entries[i];

			memory_map[i].type =
					(entry->type == LIMINE_MEMMAP_USABLE?
					 RK_BOOT_MEMORY_MAP_FREE:
					 RK_BOOT_MEMORY_MAP_USED);

			memory_map[i].start = entry->base;
			memory_map[i].end = entry->base + entry->length;

			rk_boot_info.memory_map.count++;
		}
	}

	/* get physical addresses of memory ranges */
	if ((uint64_t)rk_boot_info.framebuffer.virtual >=
	    rk_boot_info.hhdm.offset &&
	    (uint64_t)rk_boot_info.framebuffer.virtual <
	    rk_boot_info.hhdm.offset + (1ULL << 32ULL))

		rk_boot_info.framebuffer.physical =
				rk_boot_info.framebuffer.virtual -
				rk_boot_info.hhdm.offset;
}
