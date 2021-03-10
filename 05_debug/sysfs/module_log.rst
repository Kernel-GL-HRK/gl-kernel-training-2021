
.. code-block:: bash

	/modules # ./test.sh
	+ rmmod task_05_sysfs
	[ 1210.793288] Goodbye:)
	+ insmod /modules/task_05_sysfs.ko
	[ 1210.821504] 'task_05' module initialized
	+ cat /sys/class/task_05/text
	Bufer is empty:(
	+ sleep 1
	+ echo -n ChEbUrEk
	[ 1211.880053] Word 'ChEbUrEk' has converted to UPPER case successfuly
	+ cat /sys/class/task_05/text
	CHEBUREK
	+ cat /sys/class/task_05/characters_converted
	4
	+ cat /sys/class/task_05/total_characters_processed
	4
	+ cat /sys/class/task_05/total_calls
	1
	+ sleep 1
	+ echo[ 1212.963725] Word 'UsHaNi' has converted to UPPER case successfuly
	 -n UsHaNi
	+ cat /sys/class/task_05/text
	USHANI
	+ cat /sys/class/task_05/characters_converted
	3
	+ cat /sys/class/task_05/total_characters_processed
	7
	+ cat /sys/class/task_05/total_calls
	2
	/modules #
