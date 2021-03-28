#!/bin/sh -x

insmod driver.ko

hwclock
date

date -s "2007-07-07 07:07"
date


hwclock -w
hwclock


date -s "2021-03-24 21:37"
date


hwclock -w
hwclock

rmmod driver
