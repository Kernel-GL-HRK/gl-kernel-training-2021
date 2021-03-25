// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/device.h>

#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/regmap.h>
#include <linux/bcd.h>

#include "ds3132_regmap.h"


MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("RTC ds3231 driver");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");


static struct rtc_device *rtc;
static struct regmap *regmap;
static struct regmap_config regm_conf = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0x06
};



static const struct i2c_device_id ds3231x_id[] = {
	{ "ds3231x" },
	{ }
};

MODULE_DEVICE_TABLE(i2c, ds3231x_id);

static const struct of_device_id ds3231x_of_match[] = {
	{ .compatible = "dallas,ds3231x" },
	{ }
};

MODULE_DEVICE_TABLE(of, ds3231x_of_match);


int ds3231x_read_time(struct device *a_device, struct rtc_time *a_rtc_time)
{
	int must_check = 0;
	u8 arr[7];

	must_check = regmap_bulk_read(regmap, DS3231_SECONDS, arr, sizeof(arr));
	if (must_check < 0)
		dev_info(a_device, "error while read registers\n");


	a_rtc_time->tm_sec = bcd2bin(arr[DS3231_SECONDS]);
	a_rtc_time->tm_min = bcd2bin(arr[DS3231_MINUTES]);

	if (arr[DS3231_HOURS] & DS3231_HOURS_1224_MASK) {
		a_rtc_time->tm_hour =
			arr[DS3231_HOURS] & DS3231_HOURS12_AMPM_MASK ?
			bcd2bin(arr[DS3231_HOURS] & DS3231_HOURS12_MASK) + 12 :
			bcd2bin(arr[DS3231_HOURS] & DS3231_HOURS12_MASK);

	} else {
		a_rtc_time->tm_hour = bcd2bin(arr[DS3231_HOURS] &
							DS3231_HOURS24_MASK);
	}

	a_rtc_time->tm_wday = bcd2bin(arr[DS3231_WDAY]);
	a_rtc_time->tm_mday = bcd2bin(arr[DS3231_MDAY]);

	a_rtc_time->tm_mon =
		bcd2bin(arr[DS3231_MONTH_CENTURY] & DS3231_MONTH_MASK) - 1;

	a_rtc_time->tm_year = bcd2bin(arr[DS3231_YEAR]);
	if (arr[DS3231_MONTH_CENTURY] & DS3231_CENTURY_MASK)
		a_rtc_time->tm_year += 100;

	a_rtc_time->tm_yday = rtc_year_days(a_rtc_time->tm_mday,
				a_rtc_time->tm_mon, a_rtc_time->tm_year);

	dev_info(a_device,
	"year = %i, month = %i, mday = %i, hours = %i, min = %i, sec = %i\n",
		a_rtc_time->tm_year, a_rtc_time->tm_mon,  a_rtc_time->tm_mday,
		a_rtc_time->tm_hour, a_rtc_time->tm_min, a_rtc_time->tm_sec);

	return 0;
}

int ds3231x_set_time(struct device *a_device, struct rtc_time *a_rtc_time)
{
	u8 arr[7];
	int must_check = 0;

	arr[DS3231_SECONDS] = bin2bcd(a_rtc_time->tm_sec);
	arr[DS3231_MINUTES] = bin2bcd(a_rtc_time->tm_min);
	arr[DS3231_HOURS] = bin2bcd(a_rtc_time->tm_hour);

	arr[DS3231_WDAY] = bin2bcd(a_rtc_time->tm_wday);
	arr[DS3231_MDAY] = bin2bcd(a_rtc_time->tm_mday);

	arr[DS3231_MONTH_CENTURY] = bin2bcd(a_rtc_time->tm_mon + 1);

	if (a_rtc_time->tm_year >= 100) {
		a_rtc_time->tm_year -= 100;
		arr[DS3231_MONTH_CENTURY] |= DS3231_CENTURY_MASK;
	}

	arr[DS3231_YEAR] = bin2bcd(a_rtc_time->tm_year);

	must_check = regmap_bulk_write(regmap, DS3231_SECONDS,
							arr, sizeof(arr));
	if (must_check < 0) {
		dev_info(a_device, "error while set registers\n");
		return must_check;
	}

	dev_info(a_device, "time has been written\n");

	return 0;
}

static const struct rtc_class_ops ds3231x_rtc_ops = {
	.read_time = ds3231x_read_time,
	.set_time = ds3231x_set_time
};



static int ds3231x_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	s32 data = 0;
	int must_check = 0;

	data = i2c_smbus_read_byte_data(client, 0x00);
	if (data < 0) {
		dev_err(&client->dev, "%s: can`t probe device\n", __func__);
		return data;
	}

	regmap = devm_regmap_init_i2c(client, &regm_conf);
	if (IS_ERR(regmap)) {
		dev_err(&client->dev, "%s: can`t allocate regmap\n", __func__);
		return PTR_ERR(regmap);
	}

	rtc = devm_rtc_allocate_device(&client->dev);
	if (IS_ERR(rtc)) {
		dev_err(&client->dev, "%s: can`t allocate device\n", __func__);
		return PTR_ERR(rtc);
	}

	rtc->uie_unsupported = 1;
	rtc->ops = &ds3231x_rtc_ops;

	must_check = rtc_register_device(rtc);
	if (must_check < 0) {
		dev_err(&client->dev, "%s: can`t register device\n", __func__);
		return must_check;
	}


	dev_info(&client->dev, "device probed\n");

	return 0;
}

static int ds3231x_remove(struct i2c_client *client)
{
	dev_info(&client->dev, "device removed\n");
	return 0;
}


static struct i2c_driver ds3231x_driver = {
	.driver = {
		.name = "ds3231x",
		.of_match_table = ds3231x_of_match
	},
	.probe = ds3231x_probe,
	.remove = ds3231x_remove,
	.id_table = ds3231x_id,
};

module_i2c_driver(ds3231x_driver);
