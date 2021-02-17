#!/bin/sh

    rm devices.txt
    touch devices.txt

usb_devices()
{
    printf "\nUSB-UART devices :{\n" >> devices.txt
    ls /dev/ | grep ttyUSB >> devices.txt
    printf "\n}">> devices.txt
}
    
mmcblk_devices()
{
    printf "\nSD cards :{\n" >> devices.txt
    ls /dev/ | grep mmcblk >> devices.txt
    printf "\n}">> devices.txt
}

    
i2c_devices()
{
    printf "\ni2c :{\n" >> devices.txt
    ls /dev/ | grep i2c >> devices.txt
    printf "\n}">> devices.txt
}

sd_devices()
{
    printf "\nDisk :{\n" >> devices.txt
    ls /dev/ | grep sd >> devices.txt
    printf "\n}">> devices.txt
}

all_devices()
{
    usb_devices
    mmcblk_devices
    i2c_devices
    sd_devices
    
}

all_devices

cat devices.txt
