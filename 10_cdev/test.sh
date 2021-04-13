#!/bin/sh
echo {rmmod cdevdriver} > /dev/null
insmod ~/mnt/cdevdriver.ko
echo -e "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Print out empty message buffer:"
cat /dev/chat_dev0
echo -e  "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Add non ASCII char to message buffer:"
echo "Hello, Мври!" > /dev/chat_dev0
echo -e "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Add messages to message buffer:";
for i in `seq 1 5`;
do
	echo -e "Add message: 'Hello, my friend $i!'";
	echo -e ;
	echo "Hello, my friend $i!" > /dev/chat_dev0;
done;
echo -e "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Check the buffer statistics via procfs interface:";
cat /proc/users_chat_dev/buffer_statistics
echo -e "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Check the some debug information via sysfs interface:";
ls /sys/class/chat_dev/chat_dev0/
echo -e "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Read messages from the buffer:"
cat /dev/chat_dev0
echo -e "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Remove module and open it with another buffer size";
rmmod cdevdriver
insmod ~/mnt/cdevdriver.ko buffer_size=2048
echo "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Add messages to message buffer up to overflow"
for i in `seq 1 2048`;
do
	echo -e "Add message: 'Let's check it!"
	echo "Let's check it!" > /dev/chat_dev0
done;
echo -e "++++++++++++++++++++++++++++++++++++++++++++++";
echo -e "Let's clear buffer via the sysfs interface:"
cat /sys/class/chat_dev/clear_buffer
rmmod cdevdriver
