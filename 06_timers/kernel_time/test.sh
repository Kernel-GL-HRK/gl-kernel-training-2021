echo "Insert Module"
insmod khmod.ko
sleep 0.1s

for i in 0 1 2
do
  echo "Iteration - ${i}"
  delay_high=$((1 + $RANDOM % 3))
  delay_low=$((1 + $RANDOM % 10000))
  echo "sleep - ${delay_high}.${delay_low}"
  sleep ${delay_high}.${delay_low}
  echo RELATION TIME:
  cat /sys/class/kharchuk/r_diff
  echo ABSOLUTE TIME:
  cat /sys/class/kharchuk/r_prev
  echo ""
done

echo "Remove Module"
rmmod khmod.ko
