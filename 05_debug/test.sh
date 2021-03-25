
insmod main.ko
	echo "Fill the list..."
	echo "VaSyA"  > /sys/kernel/kobject_list/list
	sleep 0.2s
	echo "PeTyA"  > /sys/kernel/kobject_list/list
	sleep 0.2s
	echo "AnToN"  > /sys/kernel/kobject_list/list
	sleep 0.2s
	echo "VlaD"  > /sys/kernel/kobject_list/list

	cat  /sys/kernel/kobject_list/list
rmmod main.ko
	echo "Try to read file after deleting"
	sleep 1s
	cat  /sys/kernel/kobject_list/list
