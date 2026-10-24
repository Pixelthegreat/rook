#include "build/include/config.h"

ENTRY(_start)

PHDRS {
	limine_requests PT_LOAD;

	text PT_LOAD;
	rodata PT_LOAD;
	data PT_LOAD;

	rk_meta PT_LOAD;

	/* x86_64 stuff */
#if RK_CONFIG_ARCH == x86_64
	ap_bootstrap PT_LOAD;
	signal_bootstrap PT_LOAD;
#endif
}

SECTIONS {
	. = 0xffffffff80000000;
	_rk_start = .;

	.limine_requests ALIGN(4K) : {
		KEEP(*(.limine_requests_start))
		KEEP(*(.limine_requests))
		KEEP(*(.limine_requests_end))
	} :limine_requests

	.text ALIGN(4K) : { *(.text) } :text
	.rodata ALIGN(4K) : { *(.rodata) } :rodata
	.data ALIGN(4K) : { *(.data) } :data
	.bss ALIGN(4K) (NOLOAD) : {
		*(.bss)
		*(.bigbss)
	} :data

	.rk_meta ALIGN(4K) : {
		_rk_meta_start = .;
		*(.rk_meta)
		*(.rk_meta.1)
		*(.rk_meta.2)
		*(.rk_meta.3)
		_rk_meta_end = .;
	} :rk_meta

	/* x86_64 stuff */
#if RK_CONFIG_ARCH == x86_64
	_rk_ap_start = ALIGN(4K);
	.data.ap ALIGN(4K) : { *(.data.ap) } :ap_bootstrap
	_rk_ap_end = ALIGN(4K);
#endif

	_rk_end = .;

	/DISCARD/ : {
		*(.eh_frame*)
		*(.note .note.*)
	}
}
