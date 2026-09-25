include common.mk

.PHONY: image_limine_x86_64 all kernel clean run run_limine_x86_64 defconfig

image_limine_x86_64: pre all
	@echo == image_limine_x86_64 ==
	@./tools/image_limine_x86_64.sh

pre:
	@./tools/prebuild.sh

all: kernel

kernel:
	@echo == kernel ==
	@$(MAKE) -s -f kernel/Makefile

run: run_limine_x86_64

run_limine_x86_64:
	qemu-system-x86_64 -drive if=ide,media=cdrom,format=raw,file=build/limine-x86_64.iso -smp cpus=1 -vga std -no-reboot -no-shutdown

defconfig:
	@cp -v config.def.mk config.mk
