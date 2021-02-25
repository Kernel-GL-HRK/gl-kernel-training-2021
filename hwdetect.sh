#!/bin/bash

# Device list
allDevices=(tty i2c sd mmcblk) 

# Function to check new devices
function checkNewDevices() {
    ls /dev | grep ${1} > ${1}_updated.txt
    updated_dev=$(diff ${1}.txt ${1}_updated.txt)

    if [[ -n ${updated_dev} ]]; then
      echo "Devices are detected ${i}: ${updated_dev}"
      cp ${1}_updated.txt ${1}.txt
    fi
}

# Scanning current connected devices
for i in ${allDevices[@]}; do
  ls /dev | grep ${i} > ${i}.txt

  if [[ -s ${i}.txt ]]; then
    cat ${i}.txt
  else
    echo "No ${i} devices detected"
  fi
done


while(true); do
  for i in ${allDevices[@]}; do
    checkNewDevices "${i}"
  done
done

exit 0
