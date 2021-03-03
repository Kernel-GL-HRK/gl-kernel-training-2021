TEST_DATA="FIRST_STRING SECoNd_StRiNg Str1ng863_@KmdS" 

echo "Insert Module"
insmod khmod.ko
sleep 0.1s

echo "Show value"
cat /sys/class/kharchuk/rw
sleep 0.1s

for i in ${TEST_DATA}
do
  echo "INPUT DATA:"
  echo ${i}
  echo ""
  echo "$i" > /sys/class/kharchuk/rw
  echo "RESULT:"
  cat /sys/class/kharchuk/rw
  echo ""
  sleep 0.1s
done

echo "Remove Module"
rmmod khmod.ko
