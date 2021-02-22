ls /dev | grep ttyUSB > ttyUSB.txt
ls /dev | grep i2c > i2c.txt
ls /dev | grep sd > sd.txt
ls /dev | grep mmcblk > mmcblk.txt
ls /dev | grep nvme > nvme.txt

echo Detected devices:
cat ttyUSB.txt
cat i2c.txt
cat sd.txt
cat mmcblk.txt
cat nvme.txt


while [ 1 ]
do

    ls /dev | grep ttyUSB > ttyUSB_now.txt
    new_dev=$(diff ttyUSB.txt ttyUSB_now.txt)

    if [ "$new_dev" != "" ]
    then
        echo $new_dev
        ls /dev | grep ttyUSB > ttyUSB.txt
    fi


    ls /dev | grep i2c > i2c_now.txt
    new_dev=$(diff i2c.txt i2c_now.txt)

    if [ "$new_dev" != "" ]
    then
        echo $new_dev
        ls /dev | grep i2c > i2c.txt
    fi
     

    ls /dev | grep sd > sd_now.txt
    new_dev=$(diff sd.txt sd_now.txt)

    if [ "$new_dev" != "" ]
    then
        echo $new_dev | cut -c 10-50
        ls /dev | grep sd > sd.txt
    fi


    ls /dev | grep mmcblk > mmcblk_now.txt
    new_dev=$(diff mmcblk.txt mmcblk_now.txt)

    if [ "$new_dev" != "" ]
    then
        echo $new_dev
        ls /dev | grep mmcblk > mmcblk.txt
    fi


    ls /dev | grep nvme > nvme_now.txt
    new_dev=$(diff nvme.txt nvme_now.txt)

    if [ "$new_dev" != "" ]
    then
        echo $new_dev
        ls /dev | grep nvme > nvme.txt
    fi

    sleep 1

done
