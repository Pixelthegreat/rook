#!/bin/sh
#
# Run before the OS build
#
. build/config.sh

set -e
mkdir -pv build/include/rook

limine_protocol_url='https://codeberg.org/Limine/limine-protocol'
limine_protocol_dir='build/limine-protocol'
limine_protocol_commit='fedf97facd1c473ee8720f8dfd5a71d03490d928'

bakey_url='https://github.com/Pixelthegreat/bakey'
bakey_dir='build/bakey'

# Build config tool #
if ! [ -f build/config ] || [ tools/config.c -nt build/config ]; then
	echo ' CCLD     build/config'
	gcc -o build/config tools/config.c
fi

if ! [ -f config.mk ]; then
	cp -v config.def.mk config.mk
fi

if ! [ -f build/include/rook/config.h ] || [ config.mk -nt build/include/rook/config.h ]; then
	echo ' CONFIG'
	build/config
fi

# Download Limine protocol #
if ! [ -d "$limine_protocol_dir" ]; then
	echo Cloning limine-protocol...
	git clone "$limine_protocol_url" "$limine_protocol_dir"
	basedir="$PWD"
	cd "$limine_protocol_dir"
	git checkout "$limine_protocol_commit"
	cd "$basedir"
fi

# Download and build bakey #
if ! [ -d "$bakey_dir" ]; then
	echo Cloning bakey...
	git clone "$bakey_url" "$bakey_dir"
	basedir="$PWD"
	echo Building bakey...
	$CONFIG_TOOLCHAIN-gcc -c -g -o build/bakey.o build/bakey/src/bakey.c\
		-Ibuild/bakey/include -std=c99 -DBAKEY_FREESTANDING\
		-ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC\
		-mno-red-zone -mcmodel=kernel
	cd "$basedir"
fi
