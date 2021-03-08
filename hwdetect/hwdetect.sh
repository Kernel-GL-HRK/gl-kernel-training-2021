#!/bin/bash

declare -i sd=0
declare -i usb=0
declare -i i2c=0
declare -i ttl=0

my_dir='/dev'

echo Starting Stopwatch...
echo Press Q to exit.

while true; do


for var in  "$my_dir"/*
do
	STR=$var
	SDCARD='mmcblk0p'
	I2C='i2c'
	TTL='ttyUSB'
	USB='sdb'
	#echo "$var"
	
	case $STR in

  	*"$SDCARD"*)
    sd=$((sd+1))
    ;;
    *"$I2C"*)
    i2c=$((i2c+1))
    ;;
    *"$TTL"*)
    ttl=$((ttl+1))
    ;;
    *"$USB"*)
    usb=$((usb+1))
    ;;
	esac

done

usb=$((usb-1))

if (($usb == -1)) 
then 
	usb=$((usb+1))
fi

read -t 0.2 -N  1 input

echo "$sd - SD card devices connected; $ttl-USB_TTL connected; $i2c-I2C devices; $usb-USB Flash drive contected"


if [[ $input = "q" ]] || [[ $input = "Q" ]]; then
       
break
 
fi

usb=$((usb-usb))
i2c=$((usb-i2c))
ttl=$((usb-ttl))
sd=$((usb-sd))

done
