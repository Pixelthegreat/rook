#!/bin/sh
#
# Build a x86_64 limine based iso image
#
. build/config.sh

set -e

limine_url='https://codeberg.org/Limine/limine'
limine_dir='build/limine'
limine_branch='v9.x-binary'

# Download limine binaries #
if ! [ -d "$limine_dir" ]; then
	echo Cloning limine...
	git clone "$limine_url" "$limine_dir" --single-branch -b "$limine_branch"
	echo Building limine installer...
	gcc -o "$limine_dir/limine" "$limine_dir/limine.c"
fi

# Prepare root fs structure and generate image #
root='build/root'
iso_root="$root"
iso_output="build/limine-x86_64.iso"

echo Preparing generic root fs structure...
. tools/prepare-root.sh

mkdir -pv "$root/boot/limine"
mkdir -pv "$root/EFI/BOOT"

cp -uv tools/limine.conf "$root/boot/limine"

cp -uv "$limine_dir/BOOTX64.EFI" "$iso_root/EFI/BOOT"
cp -uv "$limine_dir/limine-uefi-cd.bin" "$iso_root/boot/limine"
cp -uv "$limine_dir/limine-bios.sys" "$iso_root/boot/limine"
cp -uv "$limine_dir/limine-bios-cd.bin" "$iso_root/boot/limine"

echo Generating limine ISO image...

xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin\
	-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus\
	-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin\
	-efi-boot-part --efi-boot-image --protective-msdos-label\
	"$iso_root" -o "$iso_output"\
	-quiet
"$limine_dir/limine" bios-install "$iso_output" --quiet
