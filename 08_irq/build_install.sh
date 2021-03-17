#!/bin/sh

export ARCH=arm
export CROSS_COMPILE='arm-linux-gnueabihf-'
export PATH=$PATH:/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/
export BUILD_KERNEL=~/gl-procamp/bbb/linux-stable/
export CURDIR=$(pwd)

cd komod/gpio_poll && make clean
make all
cd ../..

cd komod/gpio_irq && make clean
make all
cd ../..

mkdir -p /srv/nfs/busybox/komod/

cp -f komod/gpio_poll/gpio_poll_task_mod.ko /srv/nfs/busybox/komod/
cp -f komod/gpio_irq/gpio_irq_task_mod.ko /srv/nfs/busybox/komod/
