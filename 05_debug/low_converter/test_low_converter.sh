#!/bin/sh

test_string_lower="abcdefghigklmnopqrs"
test_string_upper="ABCDEFGHIGKLMNOPQRS"
test_string_large="ABCDEFGHIGKLMNOPQRSTUVWXYZ0123456789></.,][{}"
module="low_converter.ko"

echo "Testing up_converter moudule"
insmod ${module}
ls -la /sys/class/low_conv
echo "Print statistic"
cat /sys/class/low_conv/stat
echo "Read datat from converter"
cat /sys/class/low_conv/conv

echo ""
echo "Test too large string"
echo "Write data to converter:" ${test_string_large}
sleep 0.1s
echo ${test_string_large} > /sys/class/low_conv/conv
echo "Read data from converter"
cat /sys/class/low_conv/conv
echo ""
echo "Print statistic"
cat /sys/class/low_conv/stat

echo ""
echo "Test for cycling buffer"
echo "Write data to converter:" ${test_string_lower}
echo ${test_string_lower} > /sys/class/low_conv/conv
echo "Read data from converter"
cat /sys/class/low_conv/conv
echo "Print statistic"
cat /sys/class/low_conv/stat

echo ""
echo "Test for cycling buffer"
echo "Write data to converter:" ${test_string_upper}
echo ${test_string_upper} > /sys/class/low_conv/conv
echo "Read data from converter"
cat /sys/class/low_conv/conv
echo "Print statistic"
cat /sys/class/low_conv/stat

sleep 0.2s
rmmod ${module}
