#!/bin/sh
#
# Run before the OS build
#
set -e
mkdir -pv build/include/rook

limine_protocol_url='https://codeberg.org/Limine/limine-protocol'
limine_protocol_dir='build/limine-protocol'
limine_protocol_commit='fedf97facd1c473ee8720f8dfd5a71d03490d928'

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
