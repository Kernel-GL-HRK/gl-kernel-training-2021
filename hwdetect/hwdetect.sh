#!/bin/bash

# Hot plugged hardware detector
# (USB to TTL convertors, Flash drives, SD cards, I2C devices

# We'll store the previous and actual device list 
# USB-TTL devices arrays
prev_usbttl_devs=()
new_usbttl_devs=()

# Flash drives devices
prev_mmcblk_devs=()
new_mmcblk_devs=()

# SD-card arrays
prev_sd_devs=()
new_sd_devs=()

# I2C devices arrays
prev_i2c_devs=()
new_i2c_devs=()

# Separator needed to parse device list into array
IFS=';'

# In this function we'll find out which devices have been added or removed 
get_array_diffs () {
    array_result=()
    declare -a _arr1=("${!1}")
    declare -a _arr2=("${!2}")
    for i in "${_arr1[@]}"; do
        skip=
        for j in "${_arr2[@]}"; do
            [[ $i == $j ]] && { skip=1; break; }
        done
        [[ -n $skip ]] || array_result+=("$i")
    done
}

hwdetect_usbttl () {
    # Read all devices into array
    read -r -a new_usbttl_devs <<< "$(ls /dev/ | grep usbttl | tr '\n' ';')"
    
    # If size of previous array is bigger than actual, then we have new device connected
    if [[ "${#prev_usbttl_devs[@]}" > "${#new_usbttl_devs[@]}" ]]
    then
        get_array_diffs prev_usbttl_devs[@] new_usbttl_devs[@]
        echo "USB-TTL Device removed: ${array_result[@]}"
        prev_usbttl_devs=("${new_usbttl_devs[@]}")
    
    # If size of previous array is less than actual, then we have device disconnected
    elif [[ "${#prev_usbttl_devs[@]}" < "${#new_usbttl_devs[@]}" ]]
    then
        get_array_diffs new_usbttl_devs[@] prev_usbttl_devs[@]
        echo "New USB-TTL deviсe: ${array_result[@]}"
        prev_usbttl_devs=("${new_usbttl_devs[@]}")
    fi
}

hwdetect_mmcblk () {
    # Read all devices into array
    read -r -a new_mmcblk_devs <<< "$(ls /dev/ | grep mmcblk | tr '\n' ';')"

    # If size of previous array is bigger than actual, then we have new device connected
    if [[ "${#prev_mmcblk_devs[@]}" > "${#new_mmcblk_devs[@]}" ]]
    then
        get_array_diffs prev_mmcblk_devs[@] new_mmcblk_devs[@]
        echo "Flash device removed: ${array_result[@]}"
        prev_mmcblk_devs=("${new_mmcblk_devs[@]}")
    
    # If size of previous array is less than actual, then we have device disconnected
    elif [[ "${#prev_mmcblk_devs[@]}" < "${#new_mmcblk_devs[@]}" ]]
    then
        get_array_diffs new_mmcblk_devs[@] prev_mmcblk_devs[@]
        echo "New flash deviсe: ${array_result[@]}"
        prev_mmcblk_devs=("${new_mmcblk_devs[@]}")
    fi
}

hwdetect_sd () {
    # Read all devices into array
    read -r -a new_sd_devs <<< "$(ls /dev/ | grep sd | tr '\n' ';')"

    # If size of previous array is bigger than actual, then we have new device connected
    if [[ "${#prev_sd_devs[@]}" > "${#new_sd_devs[@]}" ]]
    then
        get_array_diffs prev_sd_devs[@] new_sd_devs[@]
        echo "SD device removed: ${array_result[@]}"
        prev_sd_devs=("${new_sd_devs[@]}")

    # If size of previous array is less than actual, then we have device disconnected
    elif [[ "${#prev_sd_devs[@]}" < "${#new_sd_devs[@]}" ]]
    then
        get_array_diffs new_sd_devs[@] prev_sd_devs[@]
        echo "New SD deviсe: ${array_result[@]}"
        prev_sd_devs=("${new_sd_devs[@]}")
    fi
}

hwdetect_i2c () {
    # Read all devices into array
    read -r -a new_i2c_devs <<< "$(ls /dev/ | grep i2c | tr '\n' ';')"

    # If size of previous array is bigger than actual, then we have new device connected
    if [[ "${#prev_i2c_devs[@]}" > "${#new_i2c_devs[@]}" ]]
    then
        get_array_diffs prev_i2c_devs[@] new_i2c_devs[@]
        echo "I2C Device removed: ${array_result[@]}"
        prev_i2c_devs=("${new_i2c_devs[@]}")
    
    # If size of previous array is less than actual, then we have device disconnected
    elif [[ "${#prev_i2c_devs[@]}" < "${#new_i2c_devs[@]}" ]]
    then
        get_array_diffs new_i2c_devs[@] prev_i2c_devs[@]
        echo "New I2C deviсe: ${array_result[@]}"
        prev_i2c_devs=("${new_i2c_devs[@]}")
    fi
}

# Main loop
while :
do
    hwdetect_usbttl
    hwdetect_mmcblk
    hwdetect_sd
    hwdetect_i2c

    sleep 0.5s
done

