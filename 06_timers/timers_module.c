// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/time.h>
#include <linux/printk.h>

MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Timers in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

#define DIR_NAME        "timers_info"
#define ENT_ABS_NAME    "absolute_time"
#define ENT_REL_NAME    "relation_time"

static ssize_t show_absolute_time(struct kobject *kobj,
					struct kobj_attribute *attr, char *buf);
static ssize_t show_relation_time(struct kobject *kobj,
					struct kobj_attribute *attr, char *buf);

static struct kobject *sysfs_timers_ent;

static struct kobj_attribute attr_abs_time = {
	.attr = { .name = ENT_ABS_NAME, .mode = 0444 },
	.show	= show_absolute_time
};

static struct kobj_attribute attr_rel_time = {
	.attr = { .name = ENT_REL_NAME, .mode = 0444 },
	.show	= show_relation_time
};

static int __init timers_init(void)
{
	int res = 0;

	sysfs_timers_ent = kobject_create_and_add(DIR_NAME, kernel_kobj);
	if (sysfs_timers_ent == NULL) {
		pr_err("timers_module: error! cannot create dir on sysfs\n");
		return -ENOMEM;
	}

	res = sysfs_create_file(sysfs_timers_ent, &attr_abs_time.attr);
	if (res) {
		pr_err("timers_module: error! cannot create sysfs class attribute\n");
		kobject_del(sysfs_timers_ent);
		return res;
	}

	res = sysfs_create_file(sysfs_timers_ent, &attr_rel_time.attr);
	if (res) {
		pr_err("timers_module: error! cannot create sysfs class attribute\n");
		kobject_del(sysfs_timers_ent);
		return res;
	}

	pr_info("=== timers_module loaded ===\n");
	return 0;
}

static void __exit timers_exit(void)
{

	kobject_put(sysfs_timers_ent);
	pr_info("--- timers_module unloaded ---\n");
}

ssize_t show_relation_time(struct kobject *kobj, struct kobj_attribute *attr,
							char *buf)
{
	int total_chars = 0;
	u64 current_time;
	static u64 prev_time;
	u64 diff_time;
	struct timespec64 ts_diff_time = {0};

	current_time = ktime_get_ns();
	diff_time = current_time - prev_time;

	ts_diff_time = ns_to_timespec64(diff_time);

	if (prev_time == 0) {
		total_chars = sprintf(buf, "show at first time!\n");
	} else {
		total_chars = sprintf(buf,
		"previous reading time relation: %lli.%li seconds\n",
		ts_diff_time.tv_sec, ts_diff_time.tv_nsec);
	}

	prev_time = current_time;

	return total_chars;
}

ssize_t show_absolute_time(struct kobject *kobj, struct kobj_attribute *attr,
							char *buf)
{
	int total_chars = 0;
	static struct timespec64 ts_prev_time;

	if (ts_prev_time.tv_nsec == 0) {
		total_chars = sprintf(buf, "show at first time!\n");
	} else {
		total_chars = sprintf(buf,
		"previous reading time was at: %lli.%li seconds\n",
		ts_prev_time.tv_sec, ts_prev_time.tv_nsec);
	}
	ktime_get_ts64(&ts_prev_time);

	return total_chars;
}

module_init(timers_init);
module_exit(timers_exit);
