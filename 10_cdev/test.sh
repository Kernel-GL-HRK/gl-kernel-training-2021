#!/bin/sh -x

rm -f /dev/cdev

rmmod chardev.ko
insmod chardev.ko

echo Hello Anton > /dev/char_dev
cat /dev/char_dev
cat /proc/proc_chdev/buf_volume
cat /proc/proc_chdev/buf_size
cat /sys/class/sys_char/numop
echo 1 > /sys/class/sys_char/clear
 
echo -n "It is a period of civil war. Rebel spaceships, striking from a hidden base, have won their first victory against the evil Galactic Empire. During the battle, Rebel spies managed to steal secret plans to the Empire’s ultimate weapon, the DEATH STAR, an armored space station with enough power to destroy an entire planet. Pursued by the Empire’s sinister agents, Princess Leia races home aboard her starship, custodian of the stolen plans that can save her people and restore freedom to the galaxy…." > /dev/char_dev 
sleep 0.1
cat /dev/char_dev
cat /proc/proc_chdev/buf_volume
cat /proc/proc_chdev/buf_size
cat /sys/class/sys_char/numop
echo 1 > /sys/class/sys_char/clear
sleep 0.1

rmmod chardev.ko
