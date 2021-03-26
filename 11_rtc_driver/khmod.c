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

MODULE_DESCRIPTION("RTC driver");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static uint8_t data_buff[REG_NUM];
static struct rtc_device *rtc;

int ds3231x_read_time(struct device *dev, struct rtc_time *t)
{
	int ret;

	ret = i2c_smbus_read_i2c_block_data(
			to_i2c_client(dev), DS3231_SEC_REG, REG_NUM, data_buff);
	if (ret < REG_NUM) {
		pr_err("%s: Time read error\n", THIS_MODULE->name);
		return ret;
	}

	t->tm_sec = bcd2bin(data_buff[DS3231_SEC_REG] & 0x7f);
	t->tm_min = bcd2bin(data_buff[DS3231_MIN_REG] & 0x7f);

	t->tm_hour = bcd2bin(data_buff[DS3231_HOUR_REG] & 0x3f);
	t->tm_mday = bcd2bin(data_buff[DS3231_DATE_REG] & 0x3f);

	t->tm_mon = bcd2bin(data_buff[DS3231_MONTH_REG] & 0x7f) - 1;

	t->tm_year = bcd2bin(data_buff[DS3231_YEAR_REG]);

	if (data_buff[DS3231_MONTH_REG] & (1 << 7))
		t->tm_year += 100;

	t->tm_wday = bcd2bin(data_buff[DS3231_DAY_REG] & 0x07);
	t->tm_yday = rtc_year_days(t->tm_mday, t->tm_mon, t->tm_year);

	return 0;
}

int ds3231x_write_time(struct device *dev, struct rtc_time *t)
{
	data_buff[DS3231_SEC_REG] = bin2bcd(t->tm_sec);
	data_buff[DS3231_MIN_REG] = bin2bcd(t->tm_min);
	data_buff[DS3231_HOUR_REG] = bin2bcd(t->tm_hour);
	data_buff[DS3231_DATE_REG] = bin2bcd(t->tm_mday);

	data_buff[DS3231_MONTH_REG] = bin2bcd(t->tm_mon) + 1;
	data_buff[DS3231_DAY_REG] = bin2bcd(t->tm_wday);

	if (t->tm_year > 100) {
		t->tm_year -= 100;
		data_buff[DS3231_MONTH_REG] |= (1 << 7);
	}
	data_buff[DS3231_YEAR_REG] = bin2bcd(t->tm_year);

	return i2c_smbus_write_i2c_block_data(
		to_i2c_client(dev), DS3231_SEC_REG, REG_NUM, data_buff);
}

const struct rtc_class_ops ds3231x_ops = {
	.read_time = ds3231x_read_time,
	.set_time = ds3231x_write_time,
};

int ds3231x_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;

	ret = i2c_smbus_read_byte_data(client, 0x00);
	if (ret < 0 || ret > 59)
		return ret;

	rtc = devm_rtc_allocate_device(&client->dev);
	if (IS_ERR(rtc))
		return PTR_ERR(rtc);

	rtc->uie_unsupported = true;
	rtc->ops = &ds3231x_ops;

	ret = rtc_register_device(rtc);
	if (ret)
		return ret;

	dev_info(&client->dev, "connected\n");

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
	.id_table = ds3231x_id,
};

module_i2c_driver(ds3231x_driver);
