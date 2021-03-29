#!/bin/sh

module="chdev_mod.ko"
chdev="/dev/chrdev0"
buf_clean="/sys/class/sys_chdev_entry/buf_clean"
buf_debug="/sys/class/sys_chdev_entry/buf_debug"
buf_info="/proc/procfs_chdev_entry/buf_info"
string1="abcdefghigklmnopqrs"
string2="ABCDEFGHIGKLMNOPQRS"
string_large="abcdefghigklmnopqrstuvwxyz0123456789></.,][{}"


insmod ${module}

cat $buf_debug
cat $buf_info


echo "Testing $module moudule"
insmod ${module}
echo "Print debug info"
cat $buf_debug
echo "Read buffer info"
cat $buf_info
sleep 0.1s

echo ""
echo "Test too large string"
echo "Write data to converter:" ${string_large}
sleep 0.1s
echo ${string_large} > $chdev
echo "Read data from device"
cat $chdev
echo ""
echo "Print debug info"
cat $buf_debug
sleep 0.1s

echo ""
echo "Test for cycling buffer"
echo "Write data to converter:" ${string1}
echo ${string1} > $chdev
echo "Read data from device"
cat $chdev
sleep 0.1s
echo "Print debug info"
cat $buf_debug
sleep 0.1s

echo ""
echo "Test for cycling buffer"
echo "Write data to converter:" ${string1}
sleep 0.1s
echo ${string1} > $chdev
echo "Read data from device"
cat $chdev
sleep 0.1s
echo "Print debug info"
cat $buf_debug
sleep 0.1s

echo ""
echo "Test for cycling buffer"
echo "Write data to converter:" ${string1}
sleep 0.1s
echo ${string1} > $chdev
echo "Read data from device"
cat $chdev
sleep 0.1s
echo "Print debug info"
cat $buf_debug
sleep 0.1s

echo ""
echo "Test for cleaning buffer"
echo "Write data to converter:" ${string1}
sleep 0.1s
echo ${string1} > $chdev
echo "Print debug info"
cat $buf_debug
echo "Read buffer info"
cat $buf_info
sleep 0.1s
echo "Clean buffer"
echo clean > $buf_clean
echo "Print debug info"
cat $buf_debug
echo "Read buffer info"
cat $buf_info



rmmod ${module}
