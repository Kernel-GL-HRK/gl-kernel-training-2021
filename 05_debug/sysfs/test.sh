#!/bin/sh -x

rmmod task_05_sysfs

insmod /modules/task_05_sysfs.ko

cat /sys/class/task_05/text

sleep 1
echo -n ChEbUrEk > /sys/class/task_05/text

cat /sys/class/task_05/text

cat /sys/class/task_05/characters_converted

cat /sys/class/task_05/total_characters_processed

cat /sys/class/task_05/total_calls

sleep 1
echo -n UsHaNi > /sys/class/task_05/text

cat /sys/class/task_05/text

cat /sys/class/task_05/characters_converted

cat /sys/class/task_05/total_characters_processed

cat /sys/class/task_05/total_calls
