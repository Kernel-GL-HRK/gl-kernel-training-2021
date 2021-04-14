// SPDX-License-Identifier: GPL-3.0
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/rtc.h>
#include <linux/bcd.h>

#define DS3231_REG_SECONDS      0x00
#define DS3231_REG_MINUTES      0x01
#define DS3231_REG_HOURS        0x02
#define DS3231_REG_AMPM         0x02
#define DS3231_REG_DAY          0x03
#define DS3231_REG_DATE         0x04
#define DS3231_REG_MONTH        0x05
#define DS3231_REG_YEAR         0x06
#define DS3231_REG_CR           0x0E       /* Control register */
#define DS3231_REG_SR           0x0F       /* control/status register */
/*Bit masks*/
#define DS3231_REG_SR_OSF     0x80
#define DS3231_REG_CR_nEOSC   0x80

struct ds3231x {
	struct device *dev;
	struct regmap *regmap;
	struct rtc_device *rtc;
};

static int ds3231x_check_rtc_status(struct device *dev)
{
	struct ds3231x *ds3231 = dev_get_drvdata(dev);
	int ret = 0;
	int control, stat;

	ret = regmap_read(ds3231->regmap, DS3231_REG_SR, &stat);
	if (ret)
		return ret;

	if (stat & DS3231_REG_SR_OSF)
		pr_warn("oscillator discontinuity flagged, time unreliable\n");

	stat &= ~DS3231_REG_SR_OSF;

	ret = regmap_write(ds3231->regmap, DS3231_REG_SR, stat);
	if (ret)
		return ret;

	ret = regmap_read(ds3231->regmap, DS3231_REG_CR, &control);
	if (ret)
		return ret;

	control &= ~DS3231_REG_CR_nEOSC;
	return regmap_write(ds3231->regmap, DS3231_REG_CR, control);
}

static int ds3231x_read_time(struct device *dev, struct rtc_time *time)
{
	struct ds3231x *ds3231 = dev_get_drvdata(dev);
	int ret;
	u8 buf[7];
	unsigned int year, month, day, hour, minute, second;
	unsigned int week, twelve_hr, am_pm;
	unsigned int century, add_century = 0;

	ret = regmap_bulk_read(ds3231->regmap, DS3231_REG_SECONDS, buf, 7);
	if (ret)
		return ret;

	second = buf[0];
	minute = buf[1];
	hour = buf[2];
	week = buf[3];
	day = buf[4];
	month = buf[5];
	year = buf[6];

	/* Extract additional information for AM/PM and century */

	twelve_hr = hour & 0x40;
	am_pm = hour & 0x20;
	century = month & 0x80;

	/* Write to rtc_time structure */

	time->tm_sec = bcd2bin(second);
	time->tm_min = bcd2bin(minute);
	if (twelve_hr) {
		/* Convert to 24 hr */
		if (am_pm)
			time->tm_hour = bcd2bin(hour & 0x1F) + 12;
		else
			time->tm_hour = bcd2bin(hour & 0x1F);
	} else {
		time->tm_hour = bcd2bin(hour);
	}

	/* Day of the week in linux range is 0~6 while 1~7 in RTC chip */
	time->tm_wday = bcd2bin(week) - 1;
	time->tm_mday = bcd2bin(day);
	/* linux tm_mon range:0~11, while month range is 1~12 in RTC chip */
	time->tm_mon = bcd2bin(month & 0x7F) - 1;
	if (century)
		add_century = 100;

	time->tm_year = bcd2bin(year) + add_century;

	return 0;
}

static int ds3231x_set_time(struct device *dev, struct rtc_time *time)
{
	struct ds3231x *ds3231 = dev_get_drvdata(dev);
	u8 buf[7];

	/* Extract time from rtc_time and load into ds3231*/
	buf[0] = bin2bcd(time->tm_sec);
	buf[1] = bin2bcd(time->tm_min);
	buf[2] = bin2bcd(time->tm_hour);
	/* Day of the week in linux range is 0~6 while 1~7 in RTC chip */
	buf[3] = bin2bcd(time->tm_wday + 1);
	buf[4] = bin2bcd(time->tm_mday); /* Date */
	/* linux tm_mon range:0~11, while month range is 1~12 in RTC chip */
	buf[5] = bin2bcd(time->tm_mon + 1);
	if (time->tm_year >= 100) {
		buf[5] |= 0x80;
		buf[6] = bin2bcd(time->tm_year - 100);
	} else {
		buf[6] = bin2bcd(time->tm_year);
	}

	return regmap_bulk_write(ds3231->regmap, DS3231_REG_SECONDS, buf, 7);
}

static const struct rtc_class_ops ds3231x_rtc_ops = {
	.read_time = ds3231x_read_time,
	.set_time = ds3231x_set_time,
};

static int ds3231x_probe(struct device *dev, struct regmap *regmap, int irq,
			const char *name)
{
	struct ds3231x *ds3231;
	int ret;

	ds3231 = devm_kzalloc(dev, sizeof(*ds3231), GFP_KERNEL);
	if (!ds3231)
		return -ENOMEM;

	ds3231->regmap = regmap;
	ds3231->dev = dev;
	dev_set_drvdata(dev, ds3231);

	ret = ds3231x_check_rtc_status(dev);
	if (ret)
		return ret;

	ds3231->rtc = devm_rtc_device_register(dev, name, &ds3231x_rtc_ops,
						THIS_MODULE);
	if (IS_ERR(ds3231->rtc))
		return PTR_ERR(ds3231->rtc);

	return 0;
}

static int ds3231x_i2c_probe(struct i2c_client *client,
			    const struct i2c_device_id *id)
{
	struct regmap *regmap;
	static const struct regmap_config config = {
		.reg_bits = 8,
		.val_bits = 8,
		.max_register = 0x12,
	};

	regmap = devm_regmap_init_i2c(client, &config);
	if (IS_ERR(regmap)) {
		dev_err(&client->dev, "%s: regmap allocation failed: %ld\n",
			__func__, PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

	return ds3231x_probe(&client->dev, regmap, client->irq, client->name);
}


static int ds3231x_remove(struct i2c_client *i2c)
{
	/*TODO*/
	return 0;
}


static const struct i2c_device_id ds3231x_id[] = {
	{"ds3231x"},
	{ }
};
MODULE_DEVICE_TABLE(i2c, ds3231x_id);

static const struct of_device_id ds3231x_of_match[] = {
	{.compatible = "maxim, ds3231x"},
	{ },
};

MODULE_DEVICE_TABLE(of, ds3231x_of_match);

static struct i2c_driver ds3231x_driver = {
	.driver		= {
		.name	= "ds3231x",
		.of_match_table = ds3231x_of_match,
	},
	.probe	= ds3231x_i2c_probe,
	.remove	= ds3231x_remove,
	.id_table = ds3231x_id,
};

module_i2c_driver(ds3231x_driver);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("RTC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
