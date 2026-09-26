#include "build/include/config.h"

ENTRY(_start)

PHDRS {
	limine_requests PT_LOAD;

	text PT_LOAD;
	rodata PT_LOAD;
	data PT_LOAD;

	rk_meta PT_LOAD;
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
		_rk_meta_end = .;
	} :rk_meta

	_rk_end = .;

	/DISCARD/ : {
		*(.eh_frame*)
		*(.note .note.*)
	}
}
