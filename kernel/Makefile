include common.mk
include kernel/arch/$(ARCH)/Makefile

CFLAGS += -DRK_MODULE_NAME=\"kernel\"

CFLAGS += -g -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC
CFLAGS += -Ibuild/include -Ibuild/limine-protocol/include -Ibuild/bakey/include -O0

ASFLAGS += -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC
ASFLAGS += -Ibuild/include

LDFLAGS += -g -T kernel/linker.ld -ffreestanding -nostdlib -lgcc

SOURCES += acpi.c\
	   boot.c\
	   console.c\
	   cpu.c\
	   device.c\
	   heap.c\
	   limine.c\
	   main.c\
	   meta.c\
	   module.c\
	   tty.c\
	   util.c\
	   video.c

BUILTIN_MODULES += test\
		   tty/fbcon\
		   video/bootfb

OBJECTS += $(BUILTIN_MODULES:%=build/root/usr/lib/rk/module/%.ko)
OBJECTS += build/bakey.o

SRCDIR=kernel
OUTPUT=build/root/boot/kernel

include tools/executable.mk
