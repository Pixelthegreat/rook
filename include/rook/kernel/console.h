#ifndef ROOK_KERNEL_CONSOLE_H
#define ROOK_KERNEL_CONSOLE_H

#include <rook/core.h>

/*
 * Initialize console
 */
extern void rk_console_init(void);

/*
 * Print message to console
 */
extern void rk_console_printc(char c);
extern void rk_console_prints(const char *message);

#endif /* ROOK_KERNEL_CONSOLE_H */
