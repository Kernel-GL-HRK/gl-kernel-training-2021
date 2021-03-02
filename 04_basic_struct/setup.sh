#!/bin/sh

export ARCH=arm
export CROSS_COMPILE='arm-linux-gnueabihf-'
export PATH=$PATH:/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/
export BUILD_KERNEL=~/gl/bbb/linux-stable
export CURDIR=$(pwd)
