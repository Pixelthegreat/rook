include common.mk
include kernel/arch/$(ARCH)/Makefile

CFLAGS += -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC
CFLAGS += -Ibuild/include -Ibuild/limine-protocol/include -O0

ASFLAGS += -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC
ASFLAGS += -Ibuild/include

LDFLAGS += -T kernel/linker.ld -ffreestanding -nostdlib -lgcc

SOURCES += boot.c\
	   limine.c\
	   main.c\
	   meta.c\
	   util.c

SRCDIR=kernel
OUTPUT=build/root/boot/kernel

include tools/program.mk
