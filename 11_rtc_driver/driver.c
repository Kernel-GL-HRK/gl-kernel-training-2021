// SPDX-License-Identifier: MIT and GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/device.h>

#include "ds3231.h"

MODULE_DESCRIPTION("Basic module demo: simple rtc driver");
MODULE_AUTHOR("AlexShlikhta oleksandr.shlikhta@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define NUM_OF_SIGN_REG 7

static uint8_t tmp_buf[NUM_OF_SIGN_REG];

int ds3231x_read_time(struct device *dev, struct rtc_time *t)
{
	int status = i2c_smbus_read_i2c_block_data(
		to_i2c_client(dev), DS3231_REG_SEC, NUM_OF_SIGN_REG, tmp_buf);
	if (status < NUM_OF_SIGN_REG)
		return status;

	t->tm_sec = bcd2bin(tmp_buf[DS3231_REG_SEC]);
	t->tm_min = bcd2bin(tmp_buf[DS3231_REG_MIN]);
	t->tm_hour = bcd2bin(tmp_buf[DS3231_REG_HOUR]);
	t->tm_mday = bcd2bin(tmp_buf[DS3231_REG_DATE]);

	// for RTC first month is 1, for kernel is 0
	t->tm_mon = bcd2bin((tmp_buf[DS3231_REG_MON]) & 0b01111111) - 1;

	t->tm_year = bcd2bin(tmp_buf[DS3231_REG_YEAR]);

	// converting year to kernel format
	if (tmp_buf[DS3231_REG_MON] & (1 << 7))
		t->tm_year += 100;

	t->tm_wday = bcd2bin(tmp_buf[DS3231_REG_DOW]);
	t->tm_yday = rtc_year_days(t->tm_mday, t->tm_mon, t->tm_year);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int ds3231x_write_time(struct device *dev, struct rtc_time *t)
{
	tmp_buf[DS3231_REG_SEC] = bin2bcd(t->tm_sec);
	tmp_buf[DS3231_REG_MIN] = bin2bcd(t->tm_min);
	tmp_buf[DS3231_REG_HOUR] = bin2bcd(t->tm_hour);
	tmp_buf[DS3231_REG_DATE] = bin2bcd(t->tm_mday);

	tmp_buf[DS3231_REG_MON] = bin2bcd(t->tm_mon + 1);
	tmp_buf[DS3231_REG_DOW] = bin2bcd(t->tm_wday);

	// converting year from kernel format
	if (t->tm_year > 100) {
		t->tm_year -= 100;
		// for RTC first month is 1, for kernel is 0
		tmp_buf[DS3231_REG_MON] |= (1 << 7);
	}
	tmp_buf[DS3231_REG_YEAR] = bin2bcd(t->tm_year);

	return i2c_smbus_write_i2c_block_data(
		to_i2c_client(dev), DS3231_REG_SEC, NUM_OF_SIGN_REG, tmp_buf);
}

const struct rtc_class_ops ds3231x_rtc_ops = {
	.read_time = ds3231x_read_time,
	.set_time = ds3231x_write_time,
};

///////////////////////////////////////////////////////////////////////////////////////////////////

int ds3231x_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
	int status = 0;
	int sec;
	struct rtc_device *rtc = devm_rtc_allocate_device(&drv_client->dev);

	if (IS_ERR(rtc))
		return status;

	rtc->uie_unsupported = 1; /*no IRQ line = no Update Interrupt Enable */

	rtc->ops = &ds3231x_rtc_ops;

	status = rtc_register_device(rtc);
	if (IS_ERR_VALUE(status))
		return status;

	sec = bcd2bin(i2c_smbus_read_byte_data(drv_client, DS3231_REG_SEC));

	sec = i2c_smbus_read_byte_data(drv_client, DS3231_REG_SEC);
	if (IS_ERR_VALUE(sec)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_read_byte_data() failed with error: %d\n",
			sec);
		return sec;
	}

	if (sec > 59) {
		dev_err(&drv_client->dev, "wrong i2c device found\n");
		return -1;
	}

	dev_info(&drv_client->dev, "connected\n");

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int ds3231x_remove(struct i2c_client *drv_client)
{
	dev_info(&drv_client->dev, "disconnected\n");

	return 0;
}

static const struct i2c_device_id ds3231x_id[] = { { "maxim,ds3231x", 0 }, {} };
MODULE_DEVICE_TABLE(i2c, ds3231x_id);

static const struct of_device_id ds3231x_of_match[] = {
	{ .compatible = "maxim,ds3231x" },
	{}
};
MODULE_DEVICE_TABLE(of, ds3231x_of_match);

static struct i2c_driver ds3231x_driver = {
	.driver = {
			.name = "ds3231x",
			.of_match_table = ds3231x_of_match,
	},
	.probe = ds3231x_probe,
	.remove = ds3231x_remove,
	.id_table = ds3231x_id,
};

module_i2c_driver(ds3231x_driver);
