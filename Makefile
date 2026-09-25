include common.mk

.PHONY: image_limine_x86_64 all module kernel clean run run_limine_x86_64 defconfig

image_limine_x86_64: pre all
	@./tools/image_limine_x86_64.sh

pre:
	@./tools/prebuild.sh

all: module kernel

module:
	@$(MAKE) -s -f module/Makefile

kernel:
	@$(MAKE) -s -f kernel/Makefile

clean:
	@rm -r build/obj

run: run_limine_x86_64

run_limine_x86_64:
	qemu-system-x86_64 -drive if=ide,media=cdrom,format=raw,file=build/limine-x86_64.iso -smp cpus=1 -vga std -no-reboot -no-shutdown

defconfig:
	@cp -v config.def.mk config.mk
