#!/bin/bash

# Hardware detect script

# Dummy
print_device () {
    	devices=$(ls /dev/ | grep $1)
	if [[ -n $devices ]]
	then
		echo "$2:"
		echo $devices
    	else
		echo "There is no $2"
	fi
}

print_device "ttyUSB" "USB to TTL devices"
print_device "sd" "SD cards"
print_device "mmcblk" "Flash drives"
print_device "i2c" "I2C devices"
