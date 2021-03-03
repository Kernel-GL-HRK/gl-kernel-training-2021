#!/bin/sh

test_string_lower="abcdefghigklmnopqrstuvwxyz0123456789></.,][{}"
test_string_upper="ABCDEFGHIGKLMNOPQRSTUVWXYZ0123456789></.,][{}"
module="up_converter.ko"

echo "Testing up_converter moudule"
insmod ${module}
echo "Print statistic"
cat /proc/up_conv/stat
echo "Read datat from converter"
cat /proc/up_conv/converter
echo "Write data to converter:" ${test_string_lower}
echo ${test_string_lower} > /proc/up_conv/converter
echo "Read data from converter"
cat /proc/up_conv/converter
echo "Print statistic"
cat /proc/up_conv/stat
echo "Write data to converter:" ${test_string_upper}
echo ${test_string_upper} > /proc/up_conv/converter
echo "Read data from converter"
cat /proc/up_conv/converter
echo "Print statistic"
cat /proc/up_conv/stat
sleep 0.2s
echo ${module}
rmmod ${module}
