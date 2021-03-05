#!/bin/sh

echo "rmmod task_05_sysfs"
rmmod task_05_sysfs

echo "insmod /modules/task_05_sysfs.ko"
insmod /modules/task_05_sysfs.ko

echo "cat /sys/class/task_05/text"
cat /sys/class/task_05/text

echo "echo -n ChEbUrEk > /sys/class/task_05/text"
sleep 1
echo -n ChEbUrEk > /sys/class/task_05/text

echo "cat /sys/class/task_05/text"
cat /sys/class/task_05/text

echo "cat /sys/class/task_05/characters_converted"
cat /sys/class/task_05/characters_converted

echo "cat /sys/class/task_05/total_characters_processed"
cat /sys/class/task_05/total_characters_processed

echo "cat /sys/class/task_05/total_calls"
cat /sys/class/task_05/total_calls

echo "echo -n UsHaNi > /sys/class/task_05/text"
sleep 1
echo -n UsHaNi > /sys/class/task_05/text

echo "cat /sys/class/task_05/text"
cat /sys/class/task_05/text

echo "cat /sys/class/task_05/characters_converted"
cat /sys/class/task_05/characters_converted

echo "cat /sys/class/task_05/total_characters_processed"
cat /sys/class/task_05/total_characters_processed

echo "cat /sys/class/task_05/total_calls"
cat /sys/class/task_05/total_calls
