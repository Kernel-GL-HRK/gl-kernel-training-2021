#!/bin/sh

function print_with_sleep {
    sleep $1
    echo "cat /sys/class/time_class/absolute"
    cat /sys/class/time_class/absolute
    echo "cat /sys/class/time_class/absolute"
    cat /sys/class/time_class/relative
}

rmmod kernel_time

insmod /modules/kernel_time.ko

print_with_sleep 0

for i in 1 2 3 4 5
do
    echo "iteration No"$i""

    int=$(( $RANDOM % 5 ));
    frac=$(( $RANDOM % 1000 ));

    echo "delay = "$int"."$frac" sec"

    print_with_sleep $int.$frac

done
