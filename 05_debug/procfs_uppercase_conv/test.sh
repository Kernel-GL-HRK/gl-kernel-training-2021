TEST_DATA="firs_string SECoNd_StRiNg Str1ng863_@KmdS" 

echo "Insert Module"
insmod khmod.ko
sleep 0.1s

echo "Show value"
cat /proc/kharchuk/rw
sleep 0.1s

for i in ${TEST_DATA}
do
  echo "INPUT DATA:"
  echo ${i}
  echo ""
  echo -n "$i" > /proc/kharchuk/rw
  echo "RESULT:"
  cat /proc/kharchuk/rw
  echo ""
  echo "STATS:"
  cat /proc/kharchuk/stats
  echo ""
  sleep 0.1s
done

echo "Remove Module"
rmmod khmod.ko
