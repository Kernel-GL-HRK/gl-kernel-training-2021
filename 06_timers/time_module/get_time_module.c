// SPDX-License-Identifier: GPL-3.0
/**/

#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ctype.h>

#define LEN_STR 160

static char buf_str[LEN_STR + 1];
static struct timespec64 r_last_time;
static struct timespec64 a_last_time;
static bool first_read = 1;

struct timespec64 delta_time(struct timespec64 start, struct timespec64 stop)
{
	struct timespec64 delta;

	delta.tv_sec = stop.tv_sec-start.tv_sec;
	delta.tv_nsec = stop.tv_nsec-start.tv_nsec;
	if (delta.tv_nsec < 0)
		delta.tv_nsec += 1000000000;
	return delta;
}

static ssize_t relation_delay_show(struct kobject *kobj,
	struct kobj_attribute *attr, char *buf)
{
	int ret = 0;
	struct timespec64 r_new_time, delta;

	if (first_read) {
		ret = scnprintf(buf, PAGE_SIZE,
		"RELATION DELAY FROM LAST READING \t 0s\n");
		ktime_get_real_ts64(&a_last_time);
		ktime_get_ts64(&r_new_time);
		first_read = 0;
		return ret;
	}
	ktime_get_real_ts64(&a_last_time);
	ktime_get_ts64(&r_new_time);
	delta = delta_time(r_last_time, r_new_time);
	ret = scnprintf(buf, PAGE_SIZE,
		"RELATION DELAY FROM LAST READING \t %lld.%09lds\n",
		(s64)delta.tv_sec, delta.tv_nsec);
	r_last_time = r_new_time;
	return ret;
}

static ssize_t absolute_delay_show(struct kobject *kobj,
	struct kobj_attribute *attr, char *buf)
{
	int ret = 0;

	if (first_read) {
		ret = scnprintf(buf, PAGE_SIZE,
		"ABSOLUTE TIME OF LAST READING \t 0s\n");
		ktime_get_real_ts64(&a_last_time);
		ktime_get_ts64(&r_last_time);
		first_read = 0;
		return ret;
	}
	ktime_get_ts64(&r_last_time);
	ret = scnprintf(buf, PAGE_SIZE,
		"ABSOLUTE TIME OF LAST READING \t %lld.%09lds\n",
		(s64)a_last_time.tv_sec, a_last_time.tv_nsec);
	ktime_get_real_ts64(&a_last_time);
	return ret;
}

static struct kobj_attribute relation_delay_attribute =
	__ATTR(relation_delay, 0444, relation_delay_show, NULL);

static struct kobj_attribute absolute_delay_attribute =
	__ATTR(absolute_delay, 0444, absolute_delay_show, NULL);

static struct attribute *attrs[] = {
	&relation_delay_attribute.attr,
	&absolute_delay_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *store_delay_kobj;

static int __init store_delay_module_init(void)
{
	int retval;

	memset(buf_str, '\0', LEN_STR+1);
	pr_debug("loading started\n");
	store_delay_kobj = kobject_create_and_add("PASSED_DELAY", kernel_kobj);
	if (!store_delay_kobj)
		return -ENOMEM;
	retval = sysfs_create_group(store_delay_kobj, &attr_group);
	if (retval)
		kobject_put(store_delay_kobj);

	return retval;
}

static void __exit store_delay_module_exit(void)
{
	kobject_put(store_delay_kobj);
	pr_info("module exited\n");
}

module_init(store_delay_module_init);
module_exit(store_delay_module_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Store time delay module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
