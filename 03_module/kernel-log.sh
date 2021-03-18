#!/bin/sh

# Simple script for generating logs for kernel module
# with different parameters

echo '[Hello it`s test for first Kernel module]'
echo ' '
echo '[With parameters]'
insmod main.ko user=Vlad task=3
rmmod main.ko

sleep 0.1s
echo ' '
echo '[With bad parameters]'
insmod main.ko user=Vlad task=1
rmmod main.ko

sleep 0.1s
echo ' '
echo '[Without parameters]'
insmod main.ko
sleep 0.1s
rmmod main.ko


