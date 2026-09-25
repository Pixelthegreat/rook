#include <rook/core.h>
#include <rook/kernel/boot.h>
#include <rook/kernel/console.h>

extern void main(void) {

	rk_console_init();
	rk_console_prints("Hello, world!\n");
}
