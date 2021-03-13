#!/bin/bash

function find_sdcard()
{
	echo "SDcards"
}

function find_i2c()
{
	echo "i2c"
}

function find_usb_ttl()
{
	echo "usb_ttl"
}

function find_flash()
{
	echo "flash"
}

while true; do
	PS3='Choose the type of the device: '
	options=("USB to TTL convertors" "Flash drives" "SD cards" "i2c" "Quit")
	select opt in "${options[@]}"
	do
		case $opt in
		"USB to TTL convertors")
			echo "Detected $opt:"
			find_usb_ttl
			;;
		"Flash drives")
			echo "Detected $opt:"
			find_flash
			;;
		"SD cards")
			echo "Detected $opt:"
			find_sdcard
			;;
		"i2c")
			echo "Detected $opt:"
			find_i2c
			;;
		"Quit")
			exit 0
			;;
		*) echo "Invalid option $REPLY";;
		esac
	done
done
