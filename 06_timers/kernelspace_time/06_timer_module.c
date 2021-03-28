// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/time.h>

#define CLASS_NAME   "module_06"
#define SUCCESS      0

MODULE_DESCRIPTION("Module to detect time");
MODULE_AUTHOR("Ihor Stepukh");
MODULE_LICENSE("Dual MIT/GPL");

static struct class *time_class;

static ssize_t read_diff_data(struct class *class, struct class_attribute *attr,
				char *buf)
{
	static unsigned long long last;
	unsigned long long curr;
	struct timespec64 diff;

	curr = ktime_get_ns();

	if (last != 0) {
		snprintf(buf, PAGE_SIZE, "Initial\n");
	} else {
		diff = ns_to_timespec64(curr - last);
		snprintf(buf, PAGE_SIZE, "Time diff: %lld.%06ld sec.\n",
			diff.tv_sec, diff.tv_nsec);
	}

	last = curr;
	return strlen(buf);
}

static ssize_t read_prev_data(struct class *class, struct class_attribute *attr,
				char *buf)
{
	static struct timespec64 last;

	if (last.tv_sec != 0) {
		snprintf(buf, PAGE_SIZE, "Initial\n");
	} else {
		sprintf(buf, "Previous read was %lld.%06ld sec.\n",
			last.tv_sec, last.tv_nsec);
	}

	ktime_get_ts64(&last);
	return strlen(buf);
}

struct class_attribute diff_attr = {
	.attr = { .name = "diff_attr", .mode = 0444 },
	.show	= read_diff_data,
};

struct class_attribute prev_attr = {
	.attr = { .name = "prev_attr", .mode = 0444 },
	.show	= read_prev_data,
};

static int __init module_06_init(void)
{
	int ret = SUCCESS;

	time_class = class_create(THIS_MODULE, CLASS_NAME);
	if (time_class == NULL) {
		pr_err("Unable to create class %s\n", CLASS_NAME);
		return -ENOMEM;
	}

	ret = class_create_file(time_class, &diff_attr);
	if (ret != 0) {
		pr_err("Unable to create diff_attr in class %s\n", CLASS_NAME);
		ret = -EIO;
		goto class_destroy;
	}

	ret = class_create_file(time_class, &prev_attr);
	if (ret != 0) {
		pr_err("Unable to create prev_attr in class %s\n", CLASS_NAME);
		ret = -EIO;
		goto remove_diff_attr;
	}

	ret = SUCCESS;
	pr_info("Module %s starts\n", THIS_MODULE->name);

	goto exit;

remove_diff_attr:
	class_remove_file(time_class, &diff_attr);
class_destroy:
	class_destroy(time_class);
exit:
	return ret;
}

static void __exit module_06_exit(void)
{
	class_remove_file(time_class, &prev_attr);
	class_remove_file(time_class, &diff_attr);
	class_destroy(time_class);

	pr_info("Module %s finished\n", THIS_MODULE->name);
}

module_init(module_06_init);
module_exit(module_06_exit);
