#!/bin/sh

export ARCH=arm
export CROSS_COMPILE='arm-linux-gnueabihf-'
export PATH=$PATH:/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/
export BUILD_KERNEL=~/gl-procamp/bbb/linux-stable/
export CURDIR=$(pwd)

cd komod/cdev && make clean
make all
cd ../..

mkdir -p /srv/nfs/busybox/komod/
mkdir -p /srv/nfs/busybox/home/

cp -f komod/cdev/cdev_task_mod.ko /srv/nfs/busybox/komod/
cp -f komod/test/cdev_test.sh /srv/nfs/busybox/home/
