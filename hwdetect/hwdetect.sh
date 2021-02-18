#!/bin/bash

file_orig=file.txt
file_cmp=new_file.txt
var_remove=\<
var_add=\>
device_name_max_len=32
device_dir=/dev
delay_sec=2

function process_device {
	local i flag=$1 string=$2
	
	devices=( $( diff $file_orig $file_cmp | grep $flag | \
		cut -c 3-$device_name_max_len ) )
	if [ ! -z "${devices[0]}" ]
	then
		echo $string
		for i in "${devices[@]}"
		do
			echo "$i"
		done
	fi
}

echo "Press [CTRL+C] to stop.."
ls -1 $device_dir > $file_orig

while true
do
	ls -1 $device_dir > $file_cmp

	diff $file_orig $file_cmp > /dev/null
	if [ $? -ne 0 ]
	then
		process_device $var_remove "Removed devices:"
		process_device $var_add "Added devices:"

		mv $file_cmp $file_orig
	fi

	sleep $delay_sec
done
