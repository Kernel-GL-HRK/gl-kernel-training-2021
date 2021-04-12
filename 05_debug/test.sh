
insmod main.ko
	echo "Fill the list..."
	echo "VaSyA"  > /sys/kernel/kobject_list/list
	echo "VaSyA" > /proc/procfs_list/list
	sleep 0.2s
	echo "PeTyA"  > /sys/kernel/kobject_list/list
	echo "PeTyA" > /proc/procfs_list/list
	sleep 0.2s
	echo "AnToN"  > /sys/kernel/kobject_list/list
	echo "AnToN" > /proc/procfs_list/list
	sleep 0.2s
	echo "VlaD"  > /sys/kernel/kobject_list/list
	echo "VlaD" > /proc/procfs_list/list

	echo "Now we call 3 times cat for sysfs"
	sleep 0.1s
	cat  /sys/kernel/kobject_list/list
	cat  /sys/kernel/kobject_list/list
	cat  /sys/kernel/kobject_list/list
	echo "Now we call 4 times cat for sysfs"
	sleep 0.1s
	cat /proc/procfs_list/list
	cat /proc/procfs_list/list
	cat /proc/procfs_list/list
        cat /proc/procfs_list/list
	echo "Statistics"
	sleep 0.1s
	cat /proc/procfs_list/stat
	cat /sys/kernel/kobject_list/statistics


rmmod main.ko
	echo "Try to read file after deleting"
	sleep 1s
	cat  /sys/kernel/kobject_list/list
	cat /proc/procfs_list/list
