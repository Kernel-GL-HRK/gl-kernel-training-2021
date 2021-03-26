TEST_DATA="Short should_clean  to_loooong_string"

echo [ Insert Module ]
insmod khmod.ko
sleep 0.1s

cat /sys/class/khdir/clean_show
cat /sys/class/khdir/buff_show

for i in 0 1
do
  echo [ CHDEV WRITE ]
  echo FIRST_DEMO
  echo FIRST_DEMO > /dev/khmod
  echo [ CHDEV READ ]
  cat /dev/khmod
done
rmmod khmod.ko

sleep 0.1s

insmod khmod.ko buffer_size=16
for i in ${TEST_DATA}
do
  echo [ NEW ITER ]

  echo [ CHDEV WRITE ]
  echo "$i"
  echo "$i" > /dev/khmod
  echo [ CHDEV READ ]
  cat /dev/khmod
  echo
done

sleep 0.1s

cat /sys/class/khdir/clean_show
cat /sys/class/khdir/buff_show

echo [ Remove Module ]
rmmod khmod.ko
