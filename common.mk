-include config.mk

TOOLCHAIN ?= x86_64-rook
ARCH ?= x86_64

CC=$(TOOLCHAIN)-gcc
LD=$(TOOLCHAIN)-ld

include tools/arch/$(ARCH).mk
