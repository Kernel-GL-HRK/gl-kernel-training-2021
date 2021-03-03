#!/bin/sh

# hwdetector: detects connected hardware.

detect_hardware() {

for i in $(ls /dev/ | grep $1); do
	echo "Found device: $i"
done
}

detect_hardware ttyUSB
detect_hardware mmcblk
detect_hardware sd
detect_hardware i2c
