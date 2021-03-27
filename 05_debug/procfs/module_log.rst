
.. code-block:: bash

	/modules # ./test.sh
	+ rmmod task_05_procfs
	[ 2862.701632] Goodbye:)
	+ insmod /modules/task_05_procfs.ko
	[ 2862.725715] 'task_05' module initialized
	+ cat /proc/task_05/text
	Buffer is empty:(
	+ sleep 1
	+ echo -n ChEbUrEk
	[ 2863.769291] Word 'ChEbUrEk' has converted to UPPER case successfuly
	+ cat /proc/task_05/text
	CHEBUREK
	+ cat /proc/task_05/characters_converted
	4
	+ cat /proc/task_05/total_characters_processed
	4
	+ cat /proc/task_05/total_calls
	1
	+ sleep 1
	+ echo[ 2864.855796] Word 'UsHaNi' has converted to UPPER case successfuly
	 -n UsHaNi
	+ cat /proc/task_05/text
	USHANI
	+ cat /proc/task_05/characters_converted
	3
	+ cat /proc/task_05/total_characters_processed
	7
	+ cat /proc/task_05/total_calls
	2
	/modules #
