
insmod main.ko
	echo "Fill the list..."
	echo "Vasya"  > /sys/kernel/kobject_list/list
	sleep 0.2s
	echo "Petya"  > /sys/kernel/kobject_list/list
	sleep 0.2s
	echo "Anton"  > /sys/kernel/kobject_list/list
	sleep 0.2s
	echo "Vlad"  > /sys/kernel/kobject_list/list

	cat  /sys/kernel/kobject_list/list
rmmod main.ko
	echo "Try to read file after deleting"
	sleep 1s
	cat  /sys/kernel/kobject_list/list
