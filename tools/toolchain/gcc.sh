#!/bin/sh
#
# GCC build script
#
# This script was originally written for Eclair OS, and
# adapted for Rook.
#
. build/config.sh

echo == GCC ==

set -e

GCC_DIR="build/gcc-12.2.0"
GCC_FILE="build/gcc-12.2.0.tar.xz"

GCC_SOURCE="https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.xz"

BASEDIR="$PWD"

TARGET="$CONFIG_ARCH-rook"

DESTDIR="$CONFIG_TOOLCHAIN_PATH"

echo -n "This script requires creating a directory ($DESTDIR) in order to install the toolchain to. Allow [y/N]? "
read answer_allow

if [ "$answer_allow" != "y" ] && [ "$answer_allow" != "Y" ]; then
	echo Done.
	exit 1
fi

# download and extract files #
echo Downloading sources...
curl -o "$GCC_FILE" "$GCC_SOURCE"

echo Extracting sources...
tar xf "$GCC_FILE" -C build

rm "$GCC_FILE"

# build gcc #
echo Patching gcc...
cd "$GCC_DIR"

patch -p1 -i ../../tools/toolchain/gcc.patch

echo Building gcc...

mkdir build
cd build

../configure --target="$TARGET" --prefix="$DESTDIR" --with-sysroot="$CONFIG_TOOLCHAIN_PATH/sysroot" --disable-nls --enable-languages=c,c++ --enable-shared

make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

cd "$BASEDIR"

rm -rf "$GCC_DIR"

echo This is not an error but a warning:
echo Remember to add $DESTDIR/bin to PATH before continuing!
exit 1
