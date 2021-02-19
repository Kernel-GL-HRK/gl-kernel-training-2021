#!/bin/bash

ls /dev | grep ttyUSB > tty_old.txt
ls /dev | grep sd > fd_old.txt
ls /dev | grep mmclk > usd_old.txt
ls /dev | grep i2c > i2c_old.txt


echo "Already connected devices:"

if [ "$(cat tty_old.txt)" != "" ]
then
    echo "USB to TTL convertors:"
    cat tty_old.txt
fi

if [ "$(cat fd_old.txt)" != "" ]
then
    echo "Flash drives:"
    cat fd_old.txt
fi

if [ "$(cat usd_old.txt)" != "" ]
then
    echo "SD cards:"
    cat usd_old.txt
fi

if [ "$(cat i2c_old.txt)" != "" ]
then
    echo "I2C devices:"
    cat i2c_old.txt
fi

echo "\">\" - means connected device"
echo "\"<\" - means disconnected device"


while [ 1 ]
do

    ls /dev | grep ttyUSB > tty_new.txt
    tty_tmp=$(diff tty_old.txt tty_new.txt)

    if [ "$tty_tmp" != "" ]
    then
        echo $tty_tmp
        ls /dev | grep ttyUSB > tty_old.txt
    fi


    ls /dev | grep sd > fd_new.txt
    fd_tmp=$(diff fd_old.txt fd_new.txt)

    if [ "$fd_tmp" != "" ]
    then
        echo $fd_tmp
        ls /dev | grep sd > fd_old.txt
    fi


    ls /dev | grep mmclk > usd_new.txt
    usd_tmp=$(diff usd_old.txt usd_new.txt)

    if [ "$usd_tmp" != "" ]
    then
        echo $usd_tmp
        ls /dev | grep mmclk > usd_old.txt
    fi


    ls /dev | grep i2c > i2c_new.txt
    var4=$(diff i2c_old.txt i2c_new.txt)

    if [ "$var4" != "" ]
    then
        echo $var4
        ls /dev | grep i2c > i2c_old.txt
    fi

    sleep 1

done
