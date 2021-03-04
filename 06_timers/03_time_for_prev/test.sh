#!/bin/sh

module="time_oprev.ko"
path="/sys/class/time_oprev"
device=get_ptime
delay=1.2
delay2=2.2
delay3=1.1

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
echo "Sleep $delay3 sec..."
sleep ${delay3}s
echo "Read device data:"
cat $path/$device

echo ""
echo "Read device data:"
cat $path/$device

sleep 0.2s
rmmod ${module}
