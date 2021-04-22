#!/bin/sh

module="concurency.ko"
thrds0=0
thrds1=1
thrds2=2
thrds20=20

echo "Testing $thrds0 threads"
insmod ${module} threads_amount=$thrds0
rmmod ${module}

echo ""
echo ""
echo "Testing $thrds1 threads"
insmod ${module} threads_amount=$thrds1
rmmod ${module}

echo ""
echo ""
echo "Testing $thrds2 threads"
insmod ${module} threads_amount=$thrds2
rmmod ${module}

echo ""
echo ""
echo "Testing $thrds20 threads, wait for threads will be done"
insmod ${module} threads_amount=$thrds20
sleep 2.5s
rmmod ${module}

echo ""
echo ""
echo "Testing $thrds20 threads, stopping while threads are running"
insmod ${module} threads_amount=$thrds20
rmmod ${module}
