#!/bin/sh
#
# Binutils build script
#
# This script was originally written for Eclair OS, and
# adapted for Rook.
#
. build/config.sh

echo == Binutils ==

set -e

BINUTILS_DIR="build/binutils-2.40"
BINUTILS_FILE="build/binutils-2.40.tar.xz"

BINUTILS_SOURCE="https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.xz"

BASEDIR="$PWD"

TARGET="$CONFIG_ARCH-rook"

DESTDIR="$CONFIG_TOOLCHAIN_PATH"

echo -n "This script requires creating a directory ($DESTDIR) in order to install the toolchain to. Allow [y/N]? "
read answer_allow

if [ "$answer_allow" != "y" ] && [ "$answer_allow" != "Y" ]; then
	echo Done.
	exit 1
fi

# Download and extract files #
echo Downloading sources...
curl -o "$BINUTILS_FILE" "$BINUTILS_SOURCE"

echo Extracting sources...
tar xf "$BINUTILS_FILE" -C build

rm "$BINUTILS_FILE"

# Build binutils #
echo Patching binutils...
cd "$BINUTILS_DIR"

patch -p1 -i ../../tools/toolchain/binutils.patch

echo Building binutils...

mkdir build
cd build

../configure --target="$TARGET" --prefix="$DESTDIR" --with-sysroot="$DESTDIR/sysroot" --disable-nls --disable-werror --enable-shared

mkdir -p gas/doc # Small bug with binutils-2.40: fails when attempting to copy a file to this (non-existent) directory #

make
make install

cd "$BASEDIR"

rm -rf "$BINUTILS_DIR"

echo This is not an error but a warning:
echo Remember to add $DESTDIR/bin to PATH before continuing!
exit 1
