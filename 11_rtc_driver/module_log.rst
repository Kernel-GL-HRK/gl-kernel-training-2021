
.. code-block:: bash

	/modules # ./test.sh 
	+ insmod driver.ko
	[13825.430730] ds3231x 2-0068: registered as rtc0
	[13825.436317] ds3231x 2-0068: setting system clock to 2021-03-24T21:36:26 UTC (1616621786)
	[13825.445519] ds3231x 2-0068: connected
	+ hwclock
	Wed Mar 24 21:36:26 2021  0.000000 seconds
	+ date
	Wed Mar 24 21:36:26 UTC 2021
	+ date -s '2007-07-07 07:07'
	Sat Jul  7 07:07:00 UTC 2007
	+ date
	Sat Jul  7 07:07:00 UTC 2007
	+ hwclock -w
	+ hwclock
	Sat Jul  7 07:07:00 2007  0.000000 seconds
	+ date -s '2021-03-24 21:37'
	Wed Mar 24 21:37:00 UTC 2021
	+ date
	Wed Mar 24 21:37:00 UTC 2021
	+ hwclock -w
	+ hwclock
	Wed Mar 24 21:37:00 2021  0.000000 seconds
	+ rmmod driver
	[13825.612249] ds3231x 2-0068: disconnected
	/modules # 
