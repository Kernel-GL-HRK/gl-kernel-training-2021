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

#*****************************************************

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

echo "Script will show plugged and unplugged devices. Press CTRL+C for exit."

while true
do
check_diff_dev
sleep 1
done
