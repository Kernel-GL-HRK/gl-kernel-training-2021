#!/bin/bash

TRACKED_DEVICES="ttyUSB i2c sd mmcblk"

get_devices() {
	DEVS_LIST=""
	for i in $@
	do
		DEVS_LIST+=$(ls /dev | grep $i)
		DEVS_LIST+=" "
	done
}

get_devices_cnt() {
	DEVS_CNT=0
	for i in $@
	do
		VALUE=$(ls /dev | grep $i | wc -l)
		DEVS_CNT=$((DEVS_CNT + VALUE))
	done
}

get_devices $TRACKED_DEVICES
DEVS_OLD=$DEVS_LIST

echo Devices plugged in now: $DEVS_OLD

get_devices_cnt $TRACKED_DEVICES
OLD_SIZE=$DEVS_CNT

while [ 1=1 ]
do
	get_devices $TRACKED_DEVICES
	DEVS_NEW=$DEVS_LIST

	get_devices_cnt $TRACKED_DEVICES
	NEW_SIZE=$DEVS_CNT
	
	if [[ "$OLD_SIZE" -lt "$NEW_SIZE" ]]; then
		echo Plugged in device:
	elif [[ "$OLD_SIZE" -gt "$NEW_SIZE" ]]; then
		echo Plugged out device:
	fi

	echo ${DEVS_OLD[@]} ${DEVS_NEW[@]} | tr ' ' '\n' | sort | uniq -u
	
	DEVS_OLD=$DEVS_NEW
	OLD_SIZE=$NEW_SIZE
	sleep 1
done

