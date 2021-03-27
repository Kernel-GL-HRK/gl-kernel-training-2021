// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/rtc.h>
#include <linux/bcd.h>

#define REGISTER_RW_FROM 0

#define READ_MTIME_STR "Read time mtime: sec - 0x%x, min - 0x%x, hour - 0x%x, wday - 0x%x, mday - 0x%x, mon - 0x%x, year - 0x%x\n"
#define READ_RTIME_STR "Read time rtime: sec - %d, min - %d, hour - %d, wday - %d, mday - %d, mon - %d, year - %d, yday - %d\n"
#define SET_MTIME_STR "Set time mtime: sec - 0x%x, min - 0x%x, hour - 0x%x, wday - 0x%x, mday - 0x%x, mon - 0x%x, year - 0x%x\n"
#define SET_RTIME_STR "Set time rtime: sec - %d, min - %d, hour - %d, wday - %d, mday - %d, mon - %d, year - %d, yday - %d\n"

#define COUNT_FROM_1 1
#define CENTURY 100

#define MASK_SEC 0x7F
#define MASK_MIN 0x7F
#define MASK_HOUR 0x3F
#define MASK_WDAY 0x07
#define MASK_MDAY 0x3F
#define MASK_MON 0x1F
#define MASK_CEN 0x80

/* Convert hw time to rtc */
#define M2R_TIME_SEC(time) (bcd2bin(time & MASK_SEC))
#define M2R_TIME_MIN(time) (bcd2bin(time & MASK_MIN))
#define M2R_TIME_HOUR(time) (bcd2bin(time & MASK_HOUR))
#define M2R_TIME_WDAY(time) (bcd2bin(time & MASK_WDAY) - COUNT_FROM_1)
#define M2R_TIME_MDAY(time) (bcd2bin(time & MASK_MDAY))
#define M2R_TIME_MON(time) (bcd2bin(time - (time & MASK_CEN ? MASK_CEN : 0)) \
		- COUNT_FROM_1)
#define M2R_TIME_YEAR(year, mon) (bcd2bin(year) + (mon & MASK_CEN \
			? CENTURY : 0))

/* Convert rtc time to hw */
#define R2M_TIME_SEC(time) (bin2bcd(time) & MASK_SEC)
#define R2M_TIME_MIN(time) (bin2bcd(time) & MASK_MIN)
#define R2M_TIME_HOUR(time) (bin2bcd(time) & MASK_HOUR)
#define R2M_TIME_WDAY(time) (bin2bcd(time + COUNT_FROM_1) & MASK_WDAY)
#define R2M_TIME_MDAY(time) (bin2bcd(time) & MASK_MDAY)
#define R2M_TIME_MON(mon, year) (bin2bcd(mon + COUNT_FROM_1) + (year < \
			CENTURY ? 0 : MASK_CEN))
#define R2M_TIME_YEAR(time) (bin2bcd(time - (time < CENTURY ? 0 : CENTURY)))

struct my_time {
	char sec;
	char min;
	char hour;
	char wday;
	char mday;
	char mon;
	char year;
} __packed;

static struct regmap *my_regmap;

struct regmap_config regmap_conf = {
	.reg_bits = 8, /* number of bits in register addr */
	.val_bits = 8, /* number of bits in register value */
	.max_register = 0x12 /* maximum valid register address */
};

static const struct i2c_device_id ds3231x_id[] = {
	{ "ds3231x" },
	{ },
};

MODULE_DEVICE_TABLE(i2c, ds3231x_id);

static const struct of_device_id ds3231x_of_match[] = {
	{ .compatible = "maxim,ds3231x" }, /* ds1307 already exists */
	{ }
};

static struct rtc_device *my_rtc_dev;

int my_rtc_read_time(struct device *dev, struct rtc_time *rtime)
{
	int ret;
	struct my_time mtime;

	ret = regmap_raw_read(my_regmap, REGISTER_RW_FROM, (char *)(&mtime),
			sizeof(mtime));
	if (ret)
		return ret;

	pr_info(READ_MTIME_STR, mtime.sec, mtime.min, mtime.hour, mtime.wday,
			mtime.mday, mtime.mon, mtime.year);

	rtime->tm_sec = M2R_TIME_SEC(mtime.sec);
	rtime->tm_min = M2R_TIME_MIN(mtime.min);
	rtime->tm_hour = M2R_TIME_HOUR(mtime.hour);
	rtime->tm_wday = M2R_TIME_WDAY(mtime.wday);
	rtime->tm_mday = M2R_TIME_MDAY(mtime.mday);
	rtime->tm_mon = M2R_TIME_MON(mtime.mon);
	rtime->tm_year = M2R_TIME_YEAR(mtime.year, mtime.mon);

	rtime->tm_yday = rtc_year_days(rtime->tm_mday, rtime->tm_mon,
			rtime->tm_year);

	pr_info(READ_RTIME_STR, rtime->tm_sec, rtime->tm_min, rtime->tm_hour,
		       rtime->tm_wday, rtime->tm_mday, rtime->tm_mon,
		       rtime->tm_year, rtime->tm_yday);

	return 0;
}

int my_rtc_set_time(struct device *dev, struct rtc_time *rtime)
{
	int ret;
	struct my_time mtime = {0};

	ret = rtc_valid_tm(rtime);
	if (ret)
		return ret;

	pr_info(SET_RTIME_STR, rtime->tm_sec, rtime->tm_min, rtime->tm_hour,
		       rtime->tm_wday, rtime->tm_mday, rtime->tm_mon,
		       rtime->tm_year, rtime->tm_yday);

	mtime.sec = R2M_TIME_SEC(rtime->tm_sec);
	mtime.min = R2M_TIME_MIN(rtime->tm_min);
	mtime.hour = R2M_TIME_HOUR(rtime->tm_hour);
	mtime.wday = R2M_TIME_WDAY(rtime->tm_wday);
	mtime.mday = R2M_TIME_MDAY(rtime->tm_mday);
	mtime.mon = R2M_TIME_MON(rtime->tm_mon, rtime->tm_year);
	mtime.year = R2M_TIME_YEAR(rtime->tm_year);

	pr_info(SET_MTIME_STR, mtime.sec, mtime.min, mtime.hour, mtime.wday,
			mtime.mday, mtime.mon, mtime.year);

	ret = regmap_raw_write(my_regmap, REGISTER_RW_FROM, (char *)(&mtime),
			sizeof(mtime));
	if (ret)
		return ret;
	pr_info("Set time successful\n");

	return 0;
}

static const struct rtc_class_ops my_rtc_ops = {
	.read_time = my_rtc_read_time,
	.set_time = my_rtc_set_time,
};

static int ds3231x_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int ret;

	pr_info("Probe start...\n");
	my_regmap = devm_regmap_init_i2c(client, &regmap_conf);

	if (IS_ERR(my_regmap))
		return PTR_ERR(my_regmap);

	/* RTC */
	my_rtc_dev = devm_rtc_allocate_device(&client->dev);

	if (IS_ERR(my_rtc_dev))
		return PTR_ERR(my_rtc_dev);

	my_rtc_dev->uie_unsupported = 1; /* no IRQ line = no Interrupte */
	my_rtc_dev->ops = &my_rtc_ops;

	ret = rtc_register_device(my_rtc_dev);

	if (ret)
		return ret;

	pr_info("Probe successful\n");

	return 0;
}

static int ds3231x_remove(struct i2c_client *client)
{
	pr_info("Release device\n");
	return 0;
}

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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");
