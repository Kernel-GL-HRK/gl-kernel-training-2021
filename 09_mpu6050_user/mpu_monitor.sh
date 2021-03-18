#!/bin/ash

I2C_BUS_ID=2
MPU_ADDR=0x69

function init_mpu() {
	i2cset -y $I2C_BUS_ID $MPU_ADDR 0x6b 0
}

function read_reg8() {
	local reg8=`i2cget -y $I2C_BUS_ID $MPU_ADDR $1  | sed -e 's/^0x//g' | tr a-z A-Z`
	echo $reg8
}
function read_reg16() {
	local addr=`echo "ibase=16; obase=10; $1" | bc`
	
	local msb=`read_reg8 $addr`
	addr=$((addr+1))
	local lsb=`read_reg8 $addr`
	echo $msb$lsb
}

init_mpu

while [ 1 ]
do
temp_code=`read_reg16 "41"`
cels_val=`echo "ibase=16; scale=3; ((-((FFFF-$temp_code)+1))/154)+24.87" | bc`
fahr_val=`echo "scale=3; $cels_val*1.8 + 32" | bc`
now=$(date +"%T")

echo "| $now | [$temp_code] | $cels_val C | $fahr_val F |"

sleep 1

done

exit 0
