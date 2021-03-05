#!/bin/sh

echo "rmmod task_05_procfs"
rmmod task_05_procfs

echo "insmod /modules/task_05_procfs.ko"
insmod /modules/task_05_procfs.ko

echo "cat /proc/task_05/text"
cat /proc/task_05/text

echo "echo -n ChEbUrEk > /proc/task_05/text"
sleep 1
echo -n ChEbUrEk > /proc/task_05/text

echo "cat /proc/task_05/text"
cat /proc/task_05/text
echo ""

echo "cat /proc/task_05/characters_converted"
cat /proc/task_05/characters_converted

echo "cat /proc/task_05/total_characters_processed"
cat /proc/task_05/total_characters_processed

echo "cat /proc/task_05/total_calls"
cat /proc/task_05/total_calls

echo "echo -n UsHaNi > /proc/task_05/text"
sleep 1
echo -n UsHaNi > /proc/task_05/text

echo "cat /proc/task_05/text"
cat /proc/task_05/text
echo ""

echo "cat /proc/task_05/characters_converted"
cat /proc/task_05/characters_converted

echo "cat /proc/task_05/total_characters_processed"
cat /proc/task_05/total_characters_processed

echo "cat /proc/task_05/total_calls"
cat /proc/task_05/total_calls
