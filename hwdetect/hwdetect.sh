#!/bin/bash

prev=$(ls /dev*)
now=$(ls /dev*)

function check_diff_dev() 
{
	now=$(ls /dev*)
	differ=$(echo " ${prev} ${now}" | tr ' ' '\n' | sort | uniq -u)
	
	if [ -n "$differ" ]
	then
     	differ_with_dev=$(printf "/dev/%s " $differ)
     	echo $differ_with_dev
	fi
	
	prev=$now
}

echo "Script will show plugged and unplugged devices. Press CTRL+C for exit."

while true
do
check_diff_dev
sleep 1
done
