#!/bin/sh

module="rtc_ds3231x.ko"

echo "Testing rtc moudule"
insmod ${module}

echo ""
echo "Set date..."
date -s "1980-03-23 11:23:00"

echo ""
echo "Set hw clock..."
hwclock --systohc

echo ""
echo "Set date..."
date -s "2021-03-23 18:49:00"

echo ""
echo "Set hw clock..."
hwclock --systohc

sleep 0.2s
rmmod ${module}
