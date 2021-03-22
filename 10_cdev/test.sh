#!/bin/sh

insmod /workdir/10_chdev/chdev.ko  buffer_size=1200

cat /proc/chdev_dir/chdev_buffer_size
cat /proc/chdev_dir/chdev_used_size
echo ""

cat  /sys/class/chdev_dir/chdev_device/chdev_statistics
echo ""


echo "Creator: Hi, Now I test anonymous text messages" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "Write can everyone!!!" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "everyone can read can the whole buffer at once!!!" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "if your new message plus old messages be bigger
        than buffer size only your new message will stored.
        Else information go to the /dev/null" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "Just enjoy this perfect character device" > /dev/chdev_device
cat /dev/chdev_device
cat /proc/chdev_dir/chdev_used_size
echo ""

cat /proc/chdev_dir/chdev_used_size
sleep 1
echo "s" > /sys/class/chdev_dir/chdev_device/chdev_clean_buff
sleep 1

cat /dev/chdev_device
echo ""
cat /proc/chdev_dir/chdev_used_size
echo ""

echo "Now you can use it" > /dev/chdev_device
cat /dev/chdev_device
echo ""

echo "TEST with large text"
echo ""

echo "Wikipedia is an online free-content encyclopedia project helping to create 
world in which everyone can freely share in the sum of all knowledge.
It is supported by the Wikimedia Foundation and based on a model of freely
editable content. The name Wikipedia is a blending of the words wiki
(a technology for creating collaborative websites, from the Hawaiian word
wiki, meaning quick) and encyclopedia. Wikipedia's articles provide links
designed to guide the user to related pages with additional information.

Wikipedia is written collaboratively by largely anonymous volunteers who write
without pay. Anyone with Internet access can write and make changes to Wikipedia
articles, except in limited cases where editing is restricted to prevent
further disruption or vandalism.

Since its creation on January 15, 2001, Wikipedia has grown into the world's
largest reference website, attracting 1.7 billion unique visitors monthly as
of November 2020. It currently has more than 56 million articles in more than
300 languages, including 6,273,845 articles in English with 144,452 active
contributors in the past month.

The fundamental principles by which Wikipedia operates are the five pillars.
The Wikipedia community has developed many policies and guidelines to improve
the encyclopedia; however, it is not a formal requirement to be familiar
with them before contributing." > /dev/chdev_device

cat /dev/chdev_device
echo ""

cat /proc/chdev_dir/chdev_used_size
cat  /sys/class/chdev_dir/chdev_device/chdev_statistics

sleep 1
rmmod chdev

