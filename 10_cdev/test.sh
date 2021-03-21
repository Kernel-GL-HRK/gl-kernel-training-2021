#!/bin/sh

insmod /workdir/10_chdev/chdev_misc.ko  buffer_size=2048

cat /proc/chdev_dir/chdev_buffer_size
cat /proc/chdev_dir/chdev_used_size
echo ""

cat  /sys/class/chdev_dir/chdev_device/chdev_statistics
echo ""


echo "Creator: Hi, Now I test simple anonymous text messages" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "Write can everyone!!!" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "everyone can read can the whole buffer at once!!!" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "if your new message plus old messages be bigger than buffer size only your new message will stored. Else information go to the /dev/null" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "Just enjoy this perfect character device" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

cat /proc/chdev_dir/chdev_used_size
sleep 1
echo "s" > /sys/class/chdev_dir/chdev_device/chdev_clean_buff
sleep 1

cat /dev/chdev_device
echo ""
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "Now you can use it" > /dev/chdev_device
cat /dev/chdev_device
echo ""

cat  /sys/class/chdev_dir/chdev_device/chdev_statistics

