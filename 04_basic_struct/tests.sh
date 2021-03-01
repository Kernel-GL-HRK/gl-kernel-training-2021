echo "Insert Module"
insmod khmod.ko
sleep 0.1s
echo "Show value"
cat /sys/class/kharchuk/rw
sleep 0.1s
echo "Try write"
echo "Hello-1"  > /sys/class/kharchuk/rw
echo "Hello-2"  > /sys/class/kharchuk/rw
echo "Hello-3"  > /sys/class/kharchuk/rw
sleep 0.1s
echo "Show value"
cat /sys/class/kharchuk/rw
sleep 0.1s
echo "Remove Module"
rmmod khmod.ko
