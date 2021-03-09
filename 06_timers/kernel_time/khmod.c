// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/time.h>

MODULE_DESCRIPTION("Demo time module");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static struct class *attr_class;
static struct timespec64 prev_read_time;
static struct timespec64 last_read_time;

static ssize_t r_diff_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	unsigned long long current_time;
	unsigned long long diff;

	current_time = ktime_get_ns();

	if (unlikely(prev_read_time.tv_nsec == 0)) {
		snprintf(buf, PAGE_SIZE, "First read\n");
	} else {
		diff = current_time - timespec64_to_ns(&prev_read_time);
		snprintf(buf, PAGE_SIZE, "Time between last read - %lld.%ld sec.\n",
			ns_to_timespec64(diff).tv_sec,
			ns_to_timespec64(diff).tv_nsec);
	}

	prev_read_time = ns_to_timespec64(current_time);
	return strlen(buf);
}

static ssize_t r_prev_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	struct tm time;
	unsigned long long t_temp;

	if (unlikely(last_read_time.tv_nsec == 0)) {
		snprintf(buf, PAGE_SIZE, "First read\n");
	} else {
		time_to_tm(last_read_time.tv_sec, 0, &time);
		t_temp = time.tm_min * 60 + time.tm_sec;
		sprintf(buf, "Previous read %lld.%ld sec. ago\n",
			t_temp, last_read_time.tv_nsec);
	}

	ktime_get_ts64(&last_read_time);
	return strlen(buf);
}

struct class_attribute class_attr_r_diff = {
	.attr = { .name = "r_diff", .mode = 0444 },
	.show	= r_diff_show,
};

struct class_attribute class_attr_r_prev = {
	.attr = { .name = "r_prev", .mode = 0444 },
	.show	= r_prev_show,
};

static int __init firstmod_init(void)
{
	int ret = 0;

	attr_class = class_create(THIS_MODULE, "kharchuk");

	if (attr_class == NULL) {
		pr_err("%s: error creating sysfs class\n", THIS_MODULE->name);
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_r_diff);
	if (ret) {
		pr_err("%s: error creating sysfs class r_diff attribute\n",
			THIS_MODULE->name);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_r_prev);
	if (ret) {
		pr_err("%s: error creating sysfs class r_prev attribute\n",
			THIS_MODULE->name);
		class_remove_file(attr_class, &class_attr_r_diff);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("%s: loaded!\n", THIS_MODULE->name);

	return 0;
}

static void __exit firstmod_exit(void)
{

	class_remove_file(attr_class, &class_attr_r_prev);
	class_remove_file(attr_class, &class_attr_r_diff);
	class_destroy(attr_class);

	pr_info("%s: removed!\n", THIS_MODULE->name);
}

module_init(firstmod_init);
module_exit(firstmod_exit);

