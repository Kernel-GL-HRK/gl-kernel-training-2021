
.. code-block:: bash

	/modules # ./test.sh 
	+ rm -f /dev/cdev
	+ rmmod cdev
	[ 2440.115176] Buffer cleared
	[ 2440.118432] Device destroyed
	[ 2440.121334] 'buf_clean' attr destroyed
	[ 2440.125100] 'buffer_size' attr  destroyed
	[ 2440.129221] 'used_buf_vol' attr  destroyed
	[ 2440.133351] class destroyed
	[ 2440.136161] chdev unregistered
	[ 2440.139257] 'cdev' module released
	+ insmod cdev.ko 'buffer_size=512'
	[ 2440.159477] Registered device 'cdev' with 511:0
	[ 2440.164094] 'cdev' module initialized
	+ cat /sys/class/messenger/journal
	Journal is empty:(
	+ cat /sys/class/messenger/buffer_size
	512
	+ cat /sys/class/messenger/used_buf_vol
	0
	+ cat /dev/cdev
	[ 2440.225074] Buffer is empty:(
	+ cat /sys/class/messenger/journal
	[ 2440.241934] File was opened at 214021 jiffies
	[ 2440.246595] File was read at 214021 jiffies
	[ 2440.250805] File was closed at 214022 jiffies
	Journal has printed successfuly:)
	+ echo -n cheburek
	+ cat /sys/class/messenger/journal
	[ 2440.271503] File was opened at 214021 jiffies
	[ 2440.275909] File was read at 214021 jiffies
	[ 2440.280277] File was closed at 214022 jiffies
	[ 2440.284654] File was opened at 214025 jiffies
	[ 2440.289060] 'cheburek' was written to buffer at 214025 jiffies
	[ 2440.294918] File was closed at 214025 jiffies
	Journal has printed successfuly:)
	+ cat /sys/class/messenger/used_buf_vol
	9
	+ cat /dev/cdev

	cheburek+ echo 

	+ cat /sys/class/messenger/buf_clean
	Buffer was cleared
	+ cat /sys/class/messenger/journal
	[ 2440.346899] File was opened at 214021 jiffies
	[ 2440.351302] File was read at 214021 jiffies
	[ 2440.355502] File was closed at 214022 jiffies
	[ 2440.359992] File was opened at 214025 jiffies
	[ 2440.364369] 'cheburek' was written to buffer at 214025 jiffies
	[ 2440.370252] File was closed at 214025 jiffies
	[ 2440.374626] File was opened at 214031 jiffies
	[ 2440.379028] File was read at 214031 jiffies
	[ 2440.383227] File was read at 214031 jiffies
	[ 2440.387441] File was closed at 214031 jiffies
	[ 2440.391815] Buffer was cleared at 214032 jiffies
	Journal has printed successfuly:)
	+ cat /sys/class/messenger/used_buf_vol
	0
	+ cat /dev/cdev
	[ 2440.421696] Buffer is empty:(
	+ echo -n 'AC/DC are an Australian rock band formed in Sydney in 1973 
	by Scottish-born brothers Malcolm and Angus Young. Their music has 
	been variously described as hard rock, blues rock, and heavy metal,
	but the band themselves call it simply rock and roll. AC/DC underwent
	several line-up changes before releasing their first album, 1975'"'"'s 
	High Voltage. Membership subsequently stabilised around the Young 
	brothers, singer Bon Scott, drummer Phil Rudd, and bassist Mark Evans. 
	Evans was fired from the band in 1977 and replaced by Cliff Williams, 
	who has appeared on every AC/DC album since 1978'"'"'s Powerage. In February 
	1980, Scott died of acute alcohol poisoning after a night of heavy drinking.'
	+ sleep 0.1
	+ cat /sys/class/messenger/journal
	[ 2440.558932] File was opened at 214021 jiffies
	[ 2440.563377] File was read at 214021 jiffies
	[ 2440.567910] File was closed at 214022 jiffies
	[ 2440.572316] File was opened at 214025 jiffies
	[ 2440.576790] 'cheburek' was written to buffer at 214025 jiffies
	[ 2440.582672] File was closed at 214025 jiffies
	[ 2440.587121] File was opened at 214031 jiffies
	[ 2440.591518] File was read at 214031 jiffies
	[ 2440.595737] File was read at 214031 jiffies
	[ 2440.600005] File was closed at 214031 jiffies
	[ 2440.604403] Buffer was cleared at 214032 jiffies
	[ 2440.609125] File was opened at 214041 jiffies
	[ 2440.613520] File was read at 214041 jiffies
	[ 2440.617787] File was closed at 214041 jiffies
	[ 2440.622184] File was opened at 214041 jiffies
	[ 2440.626625] **Input string too large at 214042 jiffies
	[ 2440.631806] **Buffer was overrided at 214042 jiffies
	[ 2440.636876] 'AC/DC are an Australian rock band formed in Sydney in 1973 
	[ 2440.636876] by Scottish-born brothers Malcolm and Angus Young. Their music has 
	[ 2440.636876] been variously described as hard rock, blues rock, and heavy metal,
	[ 2440.636876] but the band themselves call it simply rock and roll. AC/DC underwent
	[ 2440.636876] several line-up changes before releasing their first album, 1975's 
	[ 2440.636876] High Voltage. Membership subsequently stabilised around the Young 
	[ 2440.636876] brothers, singer Bon Scott, drummer Phil Rudd, and bassist Mark Evans. 
	[ 2440.636876] Evans was fired from the band in 1977 a' was written to buffer at 214042 jiffies
	[ 2440.696551] File was closed at 214042 jiffies
	Journal has printed successfuly:)
	+ cat /sys/class/messenger/used_buf_vol
	512
	+ cat /dev/cdev
	AC/DC are an Australian rock band formed in Sydney in 1973 
	by Scottish-born brothers Malcolm and Angus Young. Their music has 
	been variously described as hard rock, blues rock, and heavy metal,
	but the band themselves call it simply rock and roll. AC/DC underwent
	several line-up changes before releasing their first album, 1975's 
	High Voltage. Membership subsequently stabilised around the Young 
	brothers, singer Bon Scott, drummer Phil Rudd, and bassist Mark Evans. 
	Evans was fired from the band in 1977 a+ sleep 0.1
	+ echo -n finish
	+ cat /sys/class/messenger/journal
	[ 2440.862469] File was opened at 214021 jiffies
	[ 2440.867245] File was read at 214021 jiffies
	[ 2440.871475] File was closed at 214022 jiffies
	[ 2440.875874] File was opened at 214025 jiffies
	[ 2440.880373] 'cheburek' was written to buffer at 214025 jiffies
	[ 2440.886299] File was closed at 214025 jiffies
	[ 2440.890695] File was opened at 214031 jiffies
	[ 2440.895089] File was read at 214031 jiffies
	[ 2440.899358] File was read at 214031 jiffies
	[ 2440.903580] File was closed at 214031 jiffies
	[ 2440.908029] Buffer was cleared at 214032 jiffies
	[ 2440.912687] File was opened at 214041 jiffies
	[ 2440.917129] File was read at 214041 jiffies
	[ 2440.921351] File was closed at 214041 jiffies
	[ 2440.925745] File was opened at 214041 jiffies
	[ 2440.930186] **Input string too large at 214042 jiffies
	[ 2440.935369] **Buffer was overrided at 214042 jiffies
	[ 2440.940440] 'AC/DC are an Australian rock band formed in Sydney in 1973 
	[ 2440.940440] by Scottish-born brothers Malcolm and Angus Young. Their music has 
	[ 2440.940440] been variously described as hard rock, blues rock, and heavy metal,
	[ 2440.940440] but the band themselves call it simply rock and roll. AC/DC underwent
	[ 2440.940440] several line-up changes before releasing their first album, 1975's 
	[ 2440.940440] High Voltage. Membership subsequently stabilised around the Young 
	[ 2440.940440] brothers, singer Bon Scott, drummer Phil Rudd, and bassist Mark Evans. 
	[ 2440.940440] Evans was fired from the band in 1977 a' was written to buffer at 214042 jiffies
	[ 2441.000113] File was closed at 214042 jiffies
	[ 2441.004510] File was opened at 214072 jiffies
	[ 2441.008948] File was read at 214072 jiffies
	[ 2441.013168] File was read at 214072 jiffies
	[ 2441.017425] File was closed at 214072 jiffies
	[ 2441.021820] File was opened at 214083 jiffies
	[ 2441.026257] **Buffer was overrided at 214083 jiffies
	[ 2441.031263] 'finish' was written to buffer at 214083 jiffies
	[ 2441.037008] File was closed at 214083 jiffies
	Journal has printed successfuly:)
	+ cat /sys/class/messenger/used_buf_vol
	6
	+ cat /dev/cdev
	finish+ echo 

	/modules # 
