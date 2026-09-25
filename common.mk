-include config.mk

TOOLCHAIN ?= x86_64-elf
ARCH ?= x86_64

CC=$(TOOLCHAIN)-gcc

include tools/arch/$(ARCH).mk
