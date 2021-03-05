
.. code-block:: bash

	/modules # ./test.sh
	rmmod task_05_procfs
	[ 3207.855550] Goodbye:)
	insmod /modules/task_05_procfs.ko
	[ 3207.877571] 'task_05' module initialized
	cat /proc/task_05/text
	Buffer is empty:(
	echo -n ChEbUrEk > /proc/task_05/text
	[ 3208.926836] Word 'ChEbUrEk' has converted to UPPER case successfuly
	cat /proc/task_05/text
	CHEBUREK
	cat /proc/task_05/characters_converted
	4
	cat /proc/task_05/total_characters_processed
	4
	cat /proc/task_05/total_calls
	1
	echo -n UsHaNi > /proc/task_05/text
	[ 3210.012097] Word 'UsHaNi' has converted to UPPER case successfuly
	cat /proc/task_05/text
	USHANI
	cat /proc/task_05/characters_converted
	3
	cat /proc/task_05/total_characters_processed
	7
	cat /proc/task_05/total_calls
	2
	/modules #
