#!/bin/sh -x

rm -f /dev/cdev

rmmod cdev
insmod cdev.ko buffer_size=512

cat /sys/class/messenger/journal
cat /sys/class/messenger/buffer_size
cat /sys/class/messenger/used_buf_vol

cat /dev/cdev
cat /sys/class/messenger/journal


echo -n cheburek > /dev/cdev 
cat /sys/class/messenger/journal
cat /sys/class/messenger/used_buf_vol
cat /dev/cdev
echo ""

cat /sys/class/messenger/buf_clean
cat /sys/class/messenger/journal
cat /sys/class/messenger/used_buf_vol
cat /dev/cdev

echo -n "AC/DC are an Australian rock band formed in Sydney in 1973 
by Scottish-born brothers Malcolm and Angus Young. Their music has 
been variously described as hard rock, blues rock, and heavy metal,
but the band themselves call it simply rock and roll. AC/DC underwent
several line-up changes before releasing their first album, 1975's 
High Voltage. Membership subsequently stabilised around the Young 
brothers, singer Bon Scott, drummer Phil Rudd, and bassist Mark Evans. 
Evans was fired from the band in 1977 and replaced by Cliff Williams, 
who has appeared on every AC/DC album since 1978's Powerage. In February 
1980, Scott died of acute alcohol poisoning after a night of heavy drinking." > /dev/cdev 
sleep 0.1
cat /sys/class/messenger/journal
cat /sys/class/messenger/used_buf_vol
cat /dev/cdev

sleep 0.1

echo -n finish > /dev/cdev 
cat /sys/class/messenger/journal
cat /sys/class/messenger/used_buf_vol
cat /dev/cdev
echo ""
