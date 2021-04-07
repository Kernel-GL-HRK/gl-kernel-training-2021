#!/bin/sh

wake_device () {
        i2cset -y -m 0x40 2 0x68 0x6B 0x00
}

print_temperature () {
        temp_h=$(i2cget -y 2 0x68 0x41)
        if [ $(($temp_h)) > 127 ]; then
                temp_h=$(($temp_h - 256))
        fi
        temp_l=$(i2cget -y 2 0x68 0x42)
        temp_dec=$((256*$(($temp_h)) + $(($temp_l))))
        temp_c=$(($temp_dec/340 + 36))
        temp_f=$(($temp_c*9/5+32))
        echo
        echo [$(date +"%Y-%m-%d %H:%M:%S")]: the temperature is : $temp_c C or $temp_f F
}

wake_device
while sleep 1; do  print_temperature; done;


