
.. code-block:: bash

	/modules # ./test.sh
	rmmod task_05_sysfs
	[ 4602.794071] Goodbye:)
	insmod /modules/task_05_sysfs.ko
	[ 4602.820687] 'task_05' module initialized
	cat /sys/class/task_05/text
	Bufer is empty:(
	echo -n ChEbUrEk > /sys/class/task_05/text
	[ 4603.867490] Word 'ChEbUrEk' has converted to UPPER case successfuly
	cat /sys/class/task_05/text
	CHEBUREK
	cat /sys/class/task_05/characters_converted
	4
	cat /sys/class/task_05/total_characters_processed
	4
	cat /sys/class/task_05/total_calls
	1
	echo -n UsHaNi > /sys/class/task_05/text
	[ 4604.954805] Word 'UsHaNi' has converted to UPPER case successfuly
	cat /sys/class/task_05/text
	USHANI
	cat /sys/class/task_05/characters_converted
	3
	cat /sys/class/task_05/total_characters_processed
	7
	cat /sys/class/task_05/total_calls
	2
	/modules #
