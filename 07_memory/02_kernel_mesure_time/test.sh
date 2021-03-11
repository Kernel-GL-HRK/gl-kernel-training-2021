#!/bin/sh

module="mem_alloc.ko"
insmod ${module}


sleep 7s
rmmod ${module}
