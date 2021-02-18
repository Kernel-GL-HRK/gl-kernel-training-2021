#!/bin/bash

prev=$(ls /dev*)
now=$(ls /dev*)

prev_lsusb=$(lsusb)
now_lsusb=$(lsusb)



function check_plugged_unplugged() 
{
		count_prev=$(echo "$prev" | wc -w)
		count_now=$(echo "$now" | wc -w)
		
		if [ $count_prev -gt $count_now ]; then
			printf "\n"
			echo -n "**** UNPLUGGED ****"
			printf "\n"
			return 0
		else
			printf "\n"    		
    		echo -n "**** PLUGGED ****"
    		printf "\n"
    		return 1
		fi
}

#**************runtime check**************************

function check_usb_to_uart() 
{
	device=$(echo "$*" | grep -oE "/dev/ttyUSB.*")
	
	if [ -n "$device" ]
	then
		if [ $1 -gt 0 ]
		then 
		    tput setaf 2
		    echo -n "Detected USB_UART converter on "
		    echo $device
		    echo $(udevadm info $device | grep -oE "MODEL=.*")
		    echo $(udevadm info $device | grep -oE "DRIVER=.*")
		    tput sgr0
	    else
	        tput setaf 1
		    echo -n "Removed USB_UART converter on "
		    echo $device
		    tput sgr0
		fi 
	fi
}

function check_sd_card() 
{
	device=$(echo "$*" | grep -oE "\/dev\/mmcblk[0-9]" | awk '! a[$0]++')
	
	if [ -n "$device" ]
	then
		if [ $1 -gt 0 ]
		then 
		    tput setaf 2
		    echo -n "Detected SD card on "
		    echo $device
		    tput sgr0
	    else
	        tput setaf 1
		    echo -n "Removed SD card on "
		    echo $device
		    tput sgr0
		fi
	fi
}


function check_flash_driver() 
{
	device=$(echo "$*" | grep -oE "\/dev\/sd[a-z]" | awk '! a[$0]++')
	
	if [ -n "$device" ]
	then
		if [ $1 -gt 0 ]
		then 
		    tput setaf 2
		    echo -n "Detected FLASH DRIVER on "
		    echo $device
		    echo $(udevadm info $device | grep -oE "MODEL=.*")
		    echo $(udevadm info $device | grep -oE "VENDOR=.*")
		    echo $(udevadm info $device | grep -oE "BUS=.*")
		    tput sgr0
	    else
	        tput setaf 1
		    echo -n "Removed FLASH DRIVER on "
		    echo $device
		    tput sgr0
		fi 
	fi
}


#**************plugged devices check on start**************************

function check_plugged_usb_to_uart() 
{
	devices=$(echo "$(ls /dev/*)" | grep -oE "/dev/ttyUSB.*")
	
	if [ -n "$devices" ]
	then
	
	echo
	echo "******** USB TO UART *********"
	
	for d in $devices
	do
		tput setaf 1
	 	echo -n "Detected USB TO UART controler on "
		echo $d
		tput sgr0
		tput setaf 2
		echo $(udevadm info $d | grep -oE "MODEL=.*")
		echo $(udevadm info $d | grep -oE "DRIVER=.*")
		tput sgr0
		echo
	done
	
	echo "*****************"
	echo
	
	fi
}

function check_plugged_flash_drivers() 
{
	devices=$(echo "$(ls /dev/*)" | grep -oE "\/dev\/sd[a-z]" | awk '! a[$0]++')
	
	if [ -n "$devices" ]
	then
	
	echo
	echo "******** FLASH DRIVERS *********"
	
	for d in $devices 
	do
		tput setaf 1
	 	echo -n "Detected FLASH DRIVER controler on "
		echo $d
		tput sgr0
		tput setaf 2
		echo $(udevadm info $d | grep -oE "MODEL=.*")
		echo $(udevadm info $d | grep -oE "VENDOR=.*")
		echo -n $(udevadm info $d | grep -oE "BUS=.*")
		tput sgr0
		echo
	done
	
	echo "*****************"
	echo
	
	fi
}


function check_plugged_sd_cards() 
{
	devices=$(echo "$(ls /dev/*)" | grep -oE "\/dev\/mmcblk[0-9]" | awk '! a[$0]++')
	
	if [ -n "$devices" ]
	then
	
	echo
	echo "******** SD CARDS *********"
	
	for d in $devices 
	do
		tput setaf 2
	 	echo -n "Detected SD CARD on "
		echo $d
		tput sgr0
		echo
	done
	
	echo "*****************"
	echo
	
	fi
}

check_plugged_i2c_devices() 
{
	echo "******** I2C devices *********"
	for ((i=0; i < $(ls /dev/i2c-* | wc -l); i++));
	do
		tput setaf 2
		echo "I2C-$i"
		tput sgr0
		i2ctable=$(sudo i2cdetect -y $i) 
		echo $i2ctable | grep -oE "[0-f][0-f] "
		echo
	done
	echo "*****************"
}

#************************************

function check_diff_lsusb() 
{
	now_lsusb=$(lsusb)
	differ_lsusb=$(echo " ${prev_lsusb} ${now_lsusb}" | tr ' ' '\n' | sort | uniq -u)
	
	if [ -n "$differ_lsusb" ]
	then
		echo -n "lsusb --> "
     	echo $differ_lsusb
	fi
	prev_lsusb=$now_lsusb
}

function check_diff_dev() 
{
	now=$(ls /dev*)
	differ=$(echo " ${prev} ${now}" | tr ' ' '\n' | sort | uniq -u)
	
	if [ -n "$differ" ]
	then
		check_plugged_unplugged
		check_pl_un=$?
		
     	differ_with_dev=$(printf "/dev/%s " $differ)
     	echo $differ_with_dev
     	check_diff_lsusb
     	
     	check_usb_to_uart $check_pl_un $differ_with_dev
     	check_sd_card $check_pl_un $differ_with_dev
     	check_flash_driver $check_pl_un $differ_with_dev
	fi
	prev=$now
}

echo "List of connected devices:"
echo

check_plugged_i2c_devices
check_plugged_usb_to_uart
check_plugged_flash_drivers
check_plugged_sd_cards

echo "Next script will show plugged and unplugged devices. Press CTRL+C for exit."

while true
do
check_diff_dev
sleep 1
done
