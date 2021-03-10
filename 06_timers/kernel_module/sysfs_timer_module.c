// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/time.h>


MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Module which works with timers using sysfs");
MODULE_LICENSE("Dual BSD/GPL");


static struct kobject *time_watcher;
static struct timespec64 ts_last_call_time;


static ssize_t sysfs_diff_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	int lenght = 0;
	uint64_t curr_time = 0;
	uint64_t diff = 0;

	curr_time = ktime_get_ns();

	if (ts_last_call_time.tv_nsec == 0) {
		lenght = sprintf(buf, "Module is read for the first time.\n");
	} else {
		diff = curr_time - timespec64_to_ns(&ts_last_call_time);

		lenght = sprintf(buf, "Last read was %lld.%ld seconds ago\n",
			ns_to_timespec64(diff).tv_sec,
			ns_to_timespec64(diff).tv_nsec);
	}

	ts_last_call_time = ns_to_timespec64(curr_time);

	return lenght;
}


static ssize_t sysfs_last_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	int lenght = 0;
	struct tm date;

	if (ts_last_call_time.tv_nsec == 0) {
		lenght = sprintf(buf, "Module is read for the first time.\n");
	} else {
		time_to_tm(ts_last_call_time.tv_sec, 0, &date);
		lenght =
		sprintf(buf, "Last read was at %d:%d:%d.%ld %ld-%d-%d\n",
			date.tm_hour, date.tm_min, date.tm_sec,
			ts_last_call_time.tv_nsec,
			date.tm_year, date.tm_mon, date.tm_mday);
	}
	ktime_get_ts64(&ts_last_call_time);

	return lenght;
}


static struct kobj_attribute tw_diff_attribute =
	__ATTR(diff, 0660, sysfs_diff_show, NULL);


static struct kobj_attribute tw_last_attribute =
	__ATTR(last, 0660, sysfs_last_show, NULL);


static struct attribute *time_watcher_attrs[] = {
	&tw_diff_attribute.attr,
	&tw_last_attribute.attr,
	NULL,
};


static struct attribute_group time_watcher_attr_group = {
	.attrs = time_watcher_attrs
};

static int __init sysfs_timer_module_init(void)
{
	int ret_value = 0;
	ktime_t now;

	time_watcher = kobject_create_and_add("time_watcher", kernel_kobj);
	if (!time_watcher)
		ret_value = -ENOMEM;

	ret_value = sysfs_create_group(time_watcher, &time_watcher_attr_group);

	now = ktime_get_real();

	pr_info("It is %lld.%ld second of UNIX epoch.\n",
		ns_to_timespec64(now).tv_sec, ns_to_timespec64(now).tv_nsec);

	return ret_value;
}



static void __exit sysfs_timer_module_exit(void)
{
	kobject_put(time_watcher);

	pr_info("sysfs_timer_module says: Bye!\n");
}

module_init(sysfs_timer_module_init);
module_exit(sysfs_timer_module_exit);
