// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

#include <linux/sysfs.h>
#include <linux/device.h>

#include <linux/types.h>
#include <linux/slab.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("List data module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

#define INTERVAL_1NS
#define MS_TO_NS(x)     ((x) * NSEC_PER_MSEC)
#define INTERVAL_1NS

#define SYSFS_DIR_NAME	"timer_module"
#define ABS_ENT_NAME	"time_of_last"
#define REL_ENT_NAME	"time_since_last"

static ssize_t abs_show(struct kobject *kobj, struct kobj_attribute *attr,
					   char *buf)
{
	static u64 time_of_last;
	u64 then_sec, then_ns;

	then_sec = div64_u64_rem(time_of_last, NSEC_PER_SEC, &then_ns);

	time_of_last = ktime_get_ns();

	return sprintf(buf, "t[-1]: %llu.%09llu sec\n", then_sec, then_ns);
}

static ssize_t rel_show(struct kobject *kobj, struct kobj_attribute *attr,
					   char *buf)
{
	static u64 time_of_prev;
	u64 now_ns;
	u64 delta_sec, delta_ns;

	now_ns = ktime_get_ns();

	delta_ns = now_ns - time_of_prev;
	delta_sec = div64_u64_rem(delta_ns, NSEC_PER_SEC, &delta_ns);

	time_of_prev = now_ns;

	return sprintf(buf, "dt: %llu.%09llu sec\n", delta_sec, delta_ns);
}

static struct kobject *sysfs_kobject;
struct kobj_attribute kobj_attr_abs = {
	.attr = { .name = ABS_ENT_NAME, .mode = 0444 },
	.show	= abs_show,
};
struct kobj_attribute kobj_attr_rel = {
	.attr = { .name = REL_ENT_NAME, .mode = 0444 },
	.show	= rel_show,
};

int init_sysfs(void)
{
	int ret;

	sysfs_kobject = kobject_create_and_add(SYSFS_DIR_NAME, kernel_kobj);
	if (sysfs_kobject == NULL) {
		pr_err("%s: error creating kobject\n", __func__);
		return -ENOMEM;
	}

	ret = sysfs_create_file(sysfs_kobject, &kobj_attr_abs.attr);
	if (ret)
		goto failed_sysfs_init;


	ret = sysfs_create_file(sysfs_kobject, &kobj_attr_rel.attr);
	if (ret)
		goto failed_sysfs_init;


	pr_info("%s: success\n", __func__);
	return 0;

failed_sysfs_init:
	pr_err("%s: error creating sysfs class attribute\n", __func__);
	kobject_del(sysfs_kobject);
	return ret;
}

static int timer_module_init(void)
{
	int res;

	pr_info("%s: module starting\n",  __func__);

	res = init_sysfs();

	return res;
}

static void timer_module_exit(void)
{
	pr_info("%s: module exit\n",  __func__);

	kobject_del(sysfs_kobject);
}

module_init(timer_module_init);
module_exit(timer_module_exit);
