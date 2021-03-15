
.. code-block:: bash

	/ # date
	Sun Mar 21 15:48:15 UTC 2021
	/ # poweroff
	/ # umount: devtmpfs busy - remounted read-only
	swapoff: can't open '/etc/fstab': No such file or directory
	The system is going down NOW!
	Sent SIGTERM to all processes
	Sent SIGKILL to all processes
	Requesting system poweroff
	[   79.355029] reboot: System halted

near 4 minutes leter...

.. code-block:: bash
	Please press Enter to activate this console. 
	/ # date
	Sun Mar 21 15:52:24 UTC 2021
	/ # 


EEPROM

.. code-block:: bash
	/ # cat sys/bus/i2c/devices/2-0050/eeprom | hexdump -C
	00000000  63 68 65 62 75 72 65 6b  00 00 00 00 00 00 00 00  |cheburek........|
	00000010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
	*
	00001000
	/ # echo "eeprom rabotaet)" > sys/bus/i2c/devices/2-0050/eeprom
	/ # cat sys/bus/i2c/devices/2-0050/eeprom | hexdump -C
	00000000  65 65 70 72 6f 6d 20 72  61 62 6f 74 61 65 74 29  |eeprom rabotaet)|
	00000010  0a 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
	00000020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
	*
	00001000
	/ # 
