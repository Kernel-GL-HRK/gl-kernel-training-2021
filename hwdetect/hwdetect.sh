#!/bin/bash

#TODO: Detect plugged and unplugged irt




function plugged_i2c()
{
	i2c_devices=$(ls /dev/i2c-*)
	if [ -n "$i2c_devices" ] ; then
		echo "****Connected I2C****"
		for i2c in $i2c_devices ; do
			echo $i2c
		done
	fi
}

#credits to olegovich22, like his implementation
function plugged_flash_drivers()
{
	flash_devices=$(ls /dev/sd*)
	if [ -n "$flash_devices" ] ; then 
		echo
		echo "****Connected Flash drivers****"

		for d in $flash_devices ; do
			tput setaf 1
		 	echo -n "Detected flash driver controler on "
			echo $d
			tput sgr0
			tput setaf 2
			echo $(udevadm info $d | grep -oE "MODEL=.*")
			echo $(udevadm info $d | grep -oE "VENDOR=.*")
			echo -n $(udevadm info $d | grep -oE "BUS=.*")
			tput sgr0
			echo
		done
	fi
}


echo "****Checking connected HW****"
echo " "


plugged_i2c
plugged_flash_drivers


while true
do
	echo "Detecting plugged and unplugged hardware. TBD"
	#check in real time
sleep 3
done

