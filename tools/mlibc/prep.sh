#!/bin/sh
#
# This script downloads mlibc, patches it and installs
# the headers to the toolchain sysroot.
#
. build/config.sh

set -e
echo == Mlibc ==

mlibc_url='https://github.com/managarm/mlibc'
mlibc_dir='build/mlibc'
mlibc_version='v6.3.1'

# Download and patch mlibc #
basedir="$PWD"

if [ ! -d "$mlibc_dir" ]; then

	echo Cloning mlibc...
	git clone "$mlibc_url" "$mlibc_dir"

	cd "$mlibc_dir"
	git checkout "$mlibc_version"

	echo Patching mlibc...
	patch -p1 -i ../../tools/mlibc/mlibc.patch
else
	cd "$mlibc_dir"
fi

# Install headers #
echo Installing mlibc headers...
meson setup --cross-file="../../tools/meson-cross-compile/$CONFIG_ARCH.txt" --prefix=/usr -Dheaders_only=true headers-build
meson install --destdir="$CONFIG_TOOLCHAIN_PATH/sysroot" -C headers-build
meson install --destdir="$basedir/build/root" -C headers-build
