// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include <media/rc-core.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define BUF_SIZE 256
#define MODULE_NAME "time_fprev"
#define CLASS_NAME MODULE_NAME
#define STAMP_STRING "Time from last call - %ld.%09ld sec\n"
#define STAMP_STRING_FIRST "It's a first call, there are no previos calls\n"
#define DELAY_IN_MS 0
#define DELAY_IN_SEC 5
#define NSEC_IN_SEC 1000000000

static struct class *attr_class;

static ssize_t get_ptime_show(struct class *class, struct class_attribute *attr,
		char *st_buf)
{
	size_t len;
	long nsec;
	struct timespec spec;
	struct timespec stamp_spec;
	static ktime_t timestamp = {0};

	stamp_spec = ktime_to_timespec(timestamp);
	timestamp = ktime_get();

	/* Only for the first call */
	if (unlikely(!stamp_spec.tv_sec || !stamp_spec.tv_nsec)) {
		len = strlen(STAMP_STRING_FIRST);
		memcpy(st_buf, STAMP_STRING_FIRST, len);
		return len;
	}

	spec = ktime_to_timespec(timestamp);

	nsec = spec.tv_nsec - stamp_spec.tv_nsec;
	if (nsec < 0) {
		nsec += NSEC_IN_SEC;
		spec.tv_sec -= 1;
	}

	return snprintf(st_buf, BUF_SIZE, STAMP_STRING, spec.tv_sec -
			stamp_spec.tv_sec, nsec);

}

CLASS_ATTR_RO(get_ptime);

static int __init time_fprev_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (attr_class == NULL) {
		pr_err(MODULE_NAME ": error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_get_ptime);
	if (ret) {
		pr_err(MODULE_NAME ": error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info(MODULE_NAME ": init success\n");

	return 0;
}

static void __exit time_fprev_exit(void)
{
	class_remove_file(attr_class, &class_attr_get_ptime);
	class_destroy(attr_class);

	pr_info(MODULE_NAME ": module release!\n");
}

module_init(time_fprev_init);
module_exit(time_fprev_exit);
