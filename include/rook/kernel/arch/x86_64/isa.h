#ifndef ROOK_KERNEL_ARCH_X86_64_ISA_H
#define ROOK_KERNEL_ARCH_X86_64_ISA_H

#include <rook/core.h>

/*
 * Read and write bytes through isa ports
 */
extern void isa_outb(uint16_t address, uint8_t value);
extern uint8_t isa_inb(uint16_t address);

#endif /* ROOK_KERNEL_ARCH_X86_64_ISA_H */
