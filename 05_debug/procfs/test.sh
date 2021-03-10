#!/bin/sh -x

rmmod task_05_procfs

insmod /modules/task_05_procfs.ko

cat /proc/task_05/text

sleep 1
echo -n ChEbUrEk > /proc/task_05/text

cat /proc/task_05/text

cat /proc/task_05/characters_converted

cat /proc/task_05/total_characters_processed

cat /proc/task_05/total_calls

sleep 1
echo -n UsHaNi > /proc/task_05/text

cat /proc/task_05/text

cat /proc/task_05/characters_converted

cat /proc/task_05/total_characters_processed

cat /proc/task_05/total_calls
