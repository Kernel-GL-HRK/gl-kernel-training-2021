// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/coda.h> /* struct timespec */
#include <linux/ctype.h>
#include <linux/vmalloc.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Ivan Kryvosheia");
MODULE_DESCRIPTION("Timer module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("1.0");

#define SYSFS_DIR_NAME		"sysfs_gl"
#define SYSFS_FILE_NAME		"time2"

static ssize_t sysfs_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count);

static int init_sysfs_attr(void);
static void deinit_sysfs_attr(void);

static void init_hrtimer(void);
static void deinit_hrtimer(void);
static enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart);


ktime_t ktime;
static struct hrtimer hr_timer;

const static struct kobj_attribute sysfs_attr = {
	.attr = { .name = SYSFS_FILE_NAME, .mode = 0666 },
	.show	= sysfs_show,
	.store	= sysfs_store
};

static void init_hrtimer(void)
{
	/* set maximum possible expiration timeout */
	ktime = ktime_set(KTIME_SEC_MAX, 0);
	hrtimer_init(&hr_timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	hr_timer.function = &timer_callback;
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
}

static void deinit_hrtimer(void)
{
	hrtimer_cancel(&hr_timer);
}

static enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart)
{
	static int i; /* zero as its static */

	/* unlikely to happen */
	pr_info("callback:%d\n", ++i);

	return HRTIMER_RESTART;
}

//sysfs
struct kobject *kobj_inst;

static int init_sysfs_attr(void)
{
	int rc = 0;

	//create a directory into the /sys/kernel directory
	kobj_inst = kobject_create_and_add(SYSFS_DIR_NAME, kernel_kobj);

	if (!kobj_inst)
		return -1;

	rc = sysfs_create_file(kobj_inst, &sysfs_attr.attr);
	if (rc)
		return -1;

	return rc;
}

static void deinit_sysfs_attr(void)
{
	if (kobj_inst)
		kobject_put(kobj_inst);
}

static ssize_t sysfs_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	static int once = 1;
	static ktime_t last_read;
	ktime_t current_read;
	struct timespec time_human;

	if (once) {
		last_read = hrtimer_cb_get_time(&hr_timer);
		pr_info("Timer started\n");
		once = 0;
	} else {
		current_read = hrtimer_cb_get_time(&hr_timer);

		time_human = ktime_to_timespec(last_read);

		/* print time from last reading in format sec.nanosec */
		pr_info("Time of previous read %ld.%09ld\n", time_human.tv_sec,
			time_human.tv_nsec);

		/* save for the next time */
		last_read = current_read;

		/* restart timer */
		hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	}

	return 0;
}

static ssize_t sysfs_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	pr_info("Write operations not permitted\n");
	pr_info("Usage. After inserting module:\n");
	pr_info("1. cat /sys/kernel/sysfs_gl/%s\n", SYSFS_FILE_NAME);
	pr_info("2. wait a while...\n");
	pr_info("3. cat /sys/kernel/sysfs_gl/%s\n", SYSFS_FILE_NAME);
	return count;
}

static int __init tim_module_init(void)
{
	int rc;

	rc = init_sysfs_attr();
	if (rc) {
		deinit_sysfs_attr();
		pr_err("sysfs not initialized\n");
		return -1;
	}

	init_hrtimer();

	pr_info("ProCamp Timer Module inserted\n");
	return rc;
}

static void __exit tim_module_exit(void)
{
	deinit_sysfs_attr();

	deinit_hrtimer();

	pr_info("ProCamp Timer Module removed\n");
}

module_init(tim_module_init);
module_exit(tim_module_exit);
