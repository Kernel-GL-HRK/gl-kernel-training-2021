// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/time.h>


MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("Linux kernel time");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");


static struct kobject *kobj_t;

static ssize_t show_diff(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	int wr_size = 0;
	static u64 last_time;
	struct timespec64 ts_current_diff = {0};
	u64 current_time = ktime_get_ns();
	u64 current_diff =  current_time - last_time;

	ts_current_diff = ns_to_timespec64(current_diff);

	if (last_time == 0) {
		wr_size = sprintf(buf, "First reading!\n");
	} else {
		wr_size = sprintf(buf,
			"Since previous read passed: %lli.%li seconds\n",
			ts_current_diff.tv_sec, ts_current_diff.tv_nsec);
	}

	last_time = current_time;

	return wr_size;
}

static ssize_t show_last(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	int wr_size = 0;
	static struct timespec64 ts_last_time;

	if (ts_last_time.tv_nsec == 0) {
		wr_size = sprintf(buf, "First reading!\n");
	} else {
		wr_size = sprintf(buf,
			"Previous reading was at: %lli.%li seconds\n",
			ts_last_time.tv_sec, ts_last_time.tv_nsec);
	}
	ktime_get_ts64(&ts_last_time);

	return wr_size;
}


static struct kobj_attribute attr_tdiff = {
	.attr = { .name = "tdiff", .mode = 0444 },
	.show	= show_diff
};

static struct kobj_attribute attr_tlast = {
	.attr = { .name = "tlast", .mode = 0444 },
	.show	= show_last
};

static struct attribute *attrs_t[] = {
	&attr_tdiff.attr,
	&attr_tlast.attr,
	NULL
};

static struct attribute_group attr_group_t = {
	.attrs = attrs_t,
};

static int __init time_init(void)
{
	int retval;

	kobj_t = kobject_create_and_add("time_dir", kernel_kobj);
	if (kobj_t == NULL) {
		pr_err("Error while creating a kobj directory\n");
		return -ENOMEM;
	}

	retval = sysfs_create_group(kobj_t, &attr_group_t);
	if (retval) {
		pr_err("Error while creating a group\n");
		kobject_put(kobj_t);
		return retval;
	}

	pr_info("module loaded\n");
	return 0;
}

static void __exit time_exit(void)
{
	sysfs_remove_group(kobj_t, &attr_group_t);
	kobject_put(kobj_t);
	pr_info("module exited\n");
}

module_init(time_init);
module_exit(time_exit);
