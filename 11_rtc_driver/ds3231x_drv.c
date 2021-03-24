// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>

#include <linux/init.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/regmap.h>

#include "ds3231_regs.h"

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("ds3231 driver for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

static const struct i2c_device_id ds3231x_id[] = {
	{"ds3231x"}, {}
};
MODULE_DEVICE_TABLE(i2c, ds3231x_id);
static const struct of_device_id ds3231x_of_match[] = {
	{.compatible = "maxim,ds3231x" }, {}
};
MODULE_DEVICE_TABLE(of, ds3231x_of_match);

struct ds3231x {
	struct rtc_device *rtc;
	struct regmap *regmap;
	struct device *dev;
};

static int ds3231x_read_time(struct device *dev, struct rtc_time *time)
{
	struct ds3231x *ds3231 = dev_get_drvdata(dev);
	u8 regs[7];
	int ret;

	ret = regmap_bulk_read(ds3231->regmap, DS3231_REG_SECS, regs,
			       sizeof(regs));
	if (ret) {
		dev_err(dev, "%s, failed to read time regs: %d\n",
			__func__, ret);
		return ret;
	}

	dev_info(dev, "%s, regs values: %7ph\n",  __func__, regs);

	time->tm_sec = bcd2bin(regs[DS3231_REG_SECS] & 0x7f);
	time->tm_min = bcd2bin(regs[DS3231_REG_MIN] & 0x7f);
	time->tm_hour = bcd2bin(regs[DS3231_REG_HOUR] & 0x1f);
	if (regs[DS3231_REG_HOUR] & DS3231_BIT_12HR) {
		if (regs[DS3231_REG_HOUR] & DS3231_BIT_PM)
			time->tm_hour += 12;

	}

	time->tm_wday = bcd2bin(regs[DS3231_REG_WDAY] & 0x07) - 1;
	time->tm_mday = bcd2bin(regs[DS3231_REG_MDAY] & 0x3f);

	time->tm_mon = bcd2bin(regs[DS3231_REG_MONTH] & 0x1f) - 1;
	time->tm_year = bcd2bin(regs[DS3231_REG_YEAR]);
	if (regs[DS3231_REG_MONTH] & DS3231_BIT_CENTURY)
		time->tm_year += 100;

	dev_info(dev, "%s, time read: secs=%d, mins=%d, hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		__func__, time->tm_sec, time->tm_min,
			time->tm_hour, time->tm_mday,
		time->tm_mon, time->tm_year, time->tm_wday);

	return 0;
}

static int ds3231x_set_time(struct device *dev, struct rtc_time *time)
{
	struct ds3231x	*ds3231 = dev_get_drvdata(dev);
	int		ret;
	u8		regs[7];

	dev_info(dev, "%s, time to write: secs=%d, mins=%d, hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		__func__, time->tm_sec, time->tm_min,
		time->tm_hour, time->tm_mday,
		time->tm_mon, time->tm_year, time->tm_wday);


	regs[DS3231_REG_SECS] = bin2bcd(time->tm_sec);
	regs[DS3231_REG_MIN]  = bin2bcd(time->tm_min);
	regs[DS3231_REG_HOUR] = bin2bcd(time->tm_hour);
	regs[DS3231_REG_WDAY] = bin2bcd(time->tm_wday + 1);
	regs[DS3231_REG_MDAY] = bin2bcd(time->tm_mday);
	regs[DS3231_REG_MONTH] = bin2bcd(time->tm_mon + 1);
	if (time->tm_year >= 100) {
		regs[DS3231_REG_MONTH] |= DS3231_BIT_CENTURY;
		regs[DS3231_REG_YEAR] = bin2bcd(time->tm_year - 100);
	} else {
		regs[DS3231_REG_YEAR] = bin2bcd(time->tm_year);
	}

	dev_info(dev, "%s, regs values: %7ph\n",  __func__, regs);

	ret = regmap_bulk_write(ds3231->regmap, 0, regs,
				   sizeof(regs));
	if (ret) {
		dev_err(dev, "%s, failed to write time regs: %d\n",
			__func__, ret);
		return ret;
	}

	return 0;
}

static const struct rtc_class_ops ds3231_rtc_ops = {
	.read_time	= ds3231x_read_time,
	.set_time	= ds3231x_set_time,
};

static int ds3231x_rtc_startup(struct device *dev)
{
	struct ds3231x *ds3231 = dev_get_drvdata(dev);
	u8 ctl[2] = {0};
	int ret = 0;

	ret = regmap_bulk_read(ds3231->regmap, DS3231_REG_CONTROL, ctl, 2);
	if (ret)
		return ret;

	if (ctl[1] & DS3231_BIT_OSF)
		dev_warn(ds3231->dev, "oscillator was stopped, time value may be unreliable\n");

	ctl[1] &= ~(DS3231_BIT_OSF | DS3231_BIT_A1I | DS3231_BIT_A2I);

	ctl[0] &= ~(DS3231_BIT_A1IE | DS3231_BIT_A2IE);
	ctl[0] |= DS3231_BIT_INTCN;

	ret = regmap_bulk_write(ds3231->regmap, DS3231_REG_CONTROL, ctl, 2);

	return ret;
}

static int ds3231x_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	static const struct regmap_config config = {
		.reg_bits = 8,
		.val_bits = 8,
		.max_register = 0x13,
	};

	struct ds3231x *ds3231;
	int ret;

	ds3231 = devm_kzalloc(&client->dev, sizeof(*ds3231), GFP_KERNEL);
	if (!ds3231)
		return -ENOMEM;

	ds3231->regmap = devm_regmap_init_i2c(client, &config);
	if (IS_ERR(ds3231->regmap)) {
		dev_err(&client->dev, "%s: failed to alloc regmap: %ld\n",
			__func__, PTR_ERR(ds3231->regmap));
		return PTR_ERR(ds3231->regmap);
	}

	ds3231->dev = &client->dev;

	dev_set_drvdata(&client->dev, ds3231);

	ret = ds3231x_rtc_startup(ds3231->dev);
	if (ret) {
		dev_err(ds3231->dev, "%s, failed to start up rtc\n", __func__);
		return ret;
	}

	ds3231->rtc = devm_rtc_device_register(ds3231->dev, client->name,
					       &ds3231_rtc_ops,
						THIS_MODULE);
	if (IS_ERR(ds3231->rtc)) {
		dev_err(ds3231->dev, "%s, failed to redister rtc\n", __func__);
		return PTR_ERR(ds3231->rtc);
	}


	return 0;
}

static struct i2c_driver ds3231x_driver = {
	.driver = {
		.name = "ds3231x",
		.of_match_table = ds3231x_of_match,

},
	.probe = ds3231x_probe,
	.id_table = ds3231x_id,
};
module_i2c_driver(ds3231x_driver);
