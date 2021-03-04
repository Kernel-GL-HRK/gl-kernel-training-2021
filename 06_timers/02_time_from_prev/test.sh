#!/bin/sh

module="time_fprev.ko"
path="/sys/class/time_fprev"
device=get_ptime
delay=1.2
delay2=2.2

echo "Testing up_converter moudule"
insmod ${module}

echo ""
echo "Read device data:"
cat $path/$device

echo ""
echo "Sleep $delay sec..."
sleep ${delay}s
echo "Read device data:"
cat $path/$device

echo ""
echo "Sleep $delay2 sec..."
sleep ${delay2}s
echo "Read device data:"
cat $path/$device

echo ""
echo "Read device data:"
cat $path/$device

sleep 0.2s
rmmod ${module}
