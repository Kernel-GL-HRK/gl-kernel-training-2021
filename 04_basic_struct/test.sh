#!/bin/sh

module="basic_struct.ko"
path="/sys/kernel/hello"
device=list
string="This is the first string.\c"
string2="This is the second string.\c"
string3="This is the third string.\c"

echo "Testing up_converter moudule"
insmod ${module}

echo ""
echo "Read device data:"
cat $path/$device

echo ""
echo -e "Write device data:" $string
echo -e $string > $path/$device
echo ""

echo ""
echo -e "Write device data:" $string2
echo -e $string2 > $path/$device
echo ""

echo ""
echo "Read device data:"
cat $path/$device

echo ""
echo -e "Write device data:" $string3
echo -e $string3 > $path/$device
echo ""

echo ""
echo "Read device data:"
cat $path/$device

sleep 0.2s
rmmod ${module}
