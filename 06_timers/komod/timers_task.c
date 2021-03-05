// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 06_timers task
 *
 * Kernel loadable module which creates a kobject with representation
 * in the sysfs virtual filesystem, kobject contains sysfs attributes:
 * 'timestamp' - shows the absolute time
 * 'timestamp_last_read' - shows absolute time since last reading 'timestamp'
 * 'time_since_read' - shows time passed since previous read of 'timestamp'
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/ktime.h>

#define GL_SYSFS_DIR "gl_timers" /* folder name in /sys dir */
#define GL_SYSFS_TIMESTAMP timestamp
#define GL_SYSFS_TIMESTAMP_LAST_RD timestamp_last_read
#define GL_SYSFS_TIME_LAST_RD time_since_read

struct gl_time {
	u64 tm_sec;
	u64 tm_nsec;
};

static u64 gl_timestamp;
static struct kobject *sysfs_kobj;

static ssize_t gl_read_ts(struct kobject*, struct kobj_attribute*, char*);
static ssize_t gl_read_last_ts(struct kobject*, struct kobj_attribute*, char*);
static ssize_t gl_read_rel_time(struct kobject*, struct kobj_attribute*, char*);

static struct kobj_attribute ts_attr =
		__ATTR(GL_SYSFS_TIMESTAMP, 0664, gl_read_ts, NULL);

static struct kobj_attribute ts_last_read_attr =
		__ATTR(GL_SYSFS_TIMESTAMP_LAST_RD, 0664, gl_read_last_ts, NULL);

static struct kobj_attribute time_last_read_attr =
		__ATTR(GL_SYSFS_TIME_LAST_RD, 0664, gl_read_rel_time, NULL);

static struct attribute *attrs[] = {
		&ts_attr.attr,
		&ts_last_read_attr.attr,
		&time_last_read_attr.attr,
		NULL,   /* need to NULL terminate the list of attributes */
};

const struct attribute_group attr_group = {
		.attrs = attrs
};

static struct gl_time gl_get_time(u64 timestamp)
{
	struct gl_time tm;

	tm.tm_sec = div64_u64_rem(timestamp, NSEC_PER_SEC, &tm.tm_nsec);
	return tm;
}

static ssize_t gl_read_ts(struct kobject *kobj,
						struct kobj_attribute *attr,
						char *buf)
{
	struct gl_time tm;

	gl_timestamp = ktime_get_ns();
	tm = gl_get_time(gl_timestamp);
	return sprintf(buf, "%llu.%llus\n", tm.tm_sec, tm.tm_nsec);
}

static ssize_t gl_read_last_ts(struct kobject *kobj,
						struct kobj_attribute *attr,
						char *buf)
{
	struct gl_time tm;

	tm = gl_get_time(gl_timestamp);
	return sprintf(buf, "%llu.%llus\n", tm.tm_sec, tm.tm_nsec);
}

static ssize_t gl_read_rel_time(struct kobject *kobj,
						struct kobj_attribute *attr,
						char *buf)
{
	struct gl_time tm;
	u64 timestamp_rel = 0;

	if (gl_timestamp)
		timestamp_rel = ktime_get_ns() - gl_timestamp;

	tm = gl_get_time(timestamp_rel);
	return sprintf(buf, "%llu.%llus\n", tm.tm_sec, tm.tm_nsec);
}

static int gl_sysfs_init(void)
{
	int retval;

	sysfs_kobj = kobject_create_and_add(GL_SYSFS_DIR, kernel_kobj);
	if (!sysfs_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(sysfs_kobj, &attr_group);
	if (retval)
		kobject_put(sysfs_kobj);

	return 0;
}

static void gl_sysfs_deinit(void)
{
	kobject_put(sysfs_kobj);
}

static int __init gl_timers_init(void)
{
	int retval;

	pr_info("GL Kernel Training 2021\n");

	retval = gl_sysfs_init();
	if (retval)
		return retval;

	return 0;
}

static void __exit gl_timers_exit(void)
{
	gl_sysfs_deinit();
	pr_info("Exit from 06_timers module\n");
}

module_init(gl_timers_init);
module_exit(gl_timers_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 06_timers");
MODULE_LICENSE("GPL");
