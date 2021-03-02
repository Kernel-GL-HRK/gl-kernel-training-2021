#!/bin/sh

echo .
echo Test step 1: Insert Module without parameter
sleep 0.1s
insmod simple_module.ko
rmmod simple_module.ko
echo .
echo Test step 2: Insert Moudle with good parameter
sleep 0.1s
insmod simple_module.ko foo=0
rmmod simple_module.ko
echo .
echo Test step 3: Insert Moudle with bad parameter
sleep 0.1s
insmod simple_module.ko foo=5
rmmod simple_module.ko
