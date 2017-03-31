#!/bin/bash

export NDK_HOME=/usr/ndk/android-ndk-r10
export SYSROOT=$NDK_HOME/platforms/android-L/arch-arm/
export TOOLCHAIN=$NDK_HOME/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
export CPU=arm
export PREFIX=$(pwd)/android/$CPU

function build_x264
{
	./configure --prefix=$PREFIX \
	--enable-static \
	--disable-asm \
	--disable-shared \
	--enable-debug \
	--enable-pic \
	--host=arm-linux \
	--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
	--sysroot=$SYSROOT
	make clean
	make
	make install
}

build_x264
