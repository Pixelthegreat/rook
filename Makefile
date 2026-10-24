include common.mk

.PHONY: image_limine_x86_64 all module kernel
.PHONY: clean defconfig
.PHONY: run run_limine_x86_64
.PHONY: run_debug run_limine_x86_64_debug

QEMUARGS_LIMINE_X86_64 += -drive if=ide,media=cdrom,format=raw,file=build/limine-x86_64.iso -smp cpus=3 -vga std -no-reboot -no-shutdown

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

defconfig:
	@cp -v config.def.mk config.mk

run: run_limine_x86_64

run_limine_x86_64:
	qemu-system-x86_64 $(QEMUARGS_LIMINE_X86_64)

run_debug: run_limine_x86_64_debug

run_limine_x86_64_debug:
	qemu-system-x86_64 $(QEMUARGS_LIMINE_X86_64) -s -S
