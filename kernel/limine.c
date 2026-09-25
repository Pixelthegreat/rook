#include <limine.h>
#include <rook/core.h>
#include <rook/kernel/boot.h>

/* protocol metadata */
static void init(void);

RK_META rk_meta_boot_protocol_t protocol = {
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
}
