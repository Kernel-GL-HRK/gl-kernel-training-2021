# DS3231 rtc driver

See DS3231 module overview and references in the description of 09\_dt\_rtc.

Prerequisites for the task:
* Homework done 09\_dt\_rtc
* "L20: Embedded buses, I2C - RTC" from KBP-2020 course.

## Homework

* Change rtc record in the am335x-boneblack.dts from the 09\_dt\_rtc homework -- use "maxim,ds3231x" as compatible string. 
* Write ds3231x i2c device driver that registers itself as rtc device.
  * Use the same "maxim,ds3231x" as compatible string.
  * Implement only the `read_time` and `write_time` operations.
* Check module operations using `hwclock` and `date` user-space tools (see L20 slides).
* Add logs and format-patch of your changes in kernel (am335x-boneblack.dts)

