#!/bin/bash

file_orig=file.txt
file_cmp=new_file.txt
path=/tmp
var_remove=\<
var_add=\>
device_name_max_len=32
device_dir=/dev
delay_sec=2

function process_device {
	local i flag=$1 string=$2
	
	devices=( $( diff $path/$file_orig $path/$file_cmp | \
		grep $flag | cut -c 3-$device_name_max_len ) )
	if [ ! -z "${devices[0]}" ]
	then
		echo $string
		for i in "${devices[@]}"
		do
			echo "$i"
		done
	fi
}

function file_checking {
	local str=$1
	
	while [ -f "$path/$str" ] && [ -n "$(lsof "$path/$str")" ]
	do
		str=_$str
	done

	echo $str
}

echo "Press [CTRL+C] to stop.."

# add "_" symbol to the beggining of a file name if file in use
file_orig=$(file_checking $file_orig)
file_cmp=$(file_checking $file_cmp)

ls -1 $device_dir > $path/$file_orig

while true
do
	ls -1 $device_dir > $path/$file_cmp

	diff $path/$file_orig $path/$file_cmp > /dev/null
	if [ $? -ne 0 ]
	then
		process_device $var_remove "Removed devices:"
		process_device $var_add "Added devices:"

		mv $path/$file_cmp $path/$file_orig
	fi

	sleep $delay_sec
done
