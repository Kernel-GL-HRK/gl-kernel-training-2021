// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/time64.h>
#include <linux/time.h>

MODULE_DESCRIPTION("Basic module demo: timers");
MODULE_AUTHOR("AlexShlikhta oleksandr.shlikhta@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define CLASS_NAME      "time_class"
#define ABS_ATTR_NAME   absolute
#define REL_ATTR_NAME   relative

#define CLASS_ATTR(x)   class_attr_##x

static struct class *timer_class;

#define CLASS_ATTR_CREATE(_name, _mode, _show, _store) \
struct class_attribute class_attr_##_name = __ATTR(_name, _mode, _show, _store)

struct parts_of_nsec {
	uint16_t msec;
	uint16_t usec;
	uint16_t nsec;
};

static void divide_nsec(uint32_t t, struct parts_of_nsec *divided)
{
	divided->msec = t / 1000000;
	divided->usec = (t % 1000000) / 1000;
	divided->nsec = t % 1000;
}

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t abs_show(struct class *class, struct class_attribute *attr, char *buf)
{
	static struct timespec64 abs_timer;
	struct tm abs_time;
	struct parts_of_nsec tmp_nsec;

	if (abs_timer.tv_sec == 0) {

		ktime_get_ts64(&abs_timer);

		return sprintf(buf, "This is a first reading\n");
	}

	ktime_get_ts64(&abs_timer);

	time64_to_tm(abs_timer.tv_sec, 0, &abs_time);
	divide_nsec(abs_timer.tv_nsec, &tmp_nsec);

	return sprintf(buf, "previous reading was at %d:%d:%d:%d:%d:%d\n", abs_time.tm_hour,
										abs_time.tm_min, abs_time.tm_sec, tmp_nsec.msec,
										tmp_nsec.usec, tmp_nsec.nsec);
}

CLASS_ATTR_CREATE(ABS_ATTR_NAME, 0444, abs_show, NULL);


/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t rel_show(struct class *class, struct class_attribute *attr, char *buf)
{
	static uint64_t ns;
	uint64_t cur_ns;
	uint64_t diff;
	struct parts_of_nsec t_n;

	if (ns == 0) {

		ns = ktime_get_ns();

		return sprintf(buf, "This is a first reading\n");
	}

	cur_ns = ktime_get_ns();
	diff = cur_ns - ns;
	ns = ktime_get_ns();

	divide_nsec(ns_to_timespec64(diff).tv_nsec, &t_n);

	return sprintf(buf, "previous reading was %lld:%d:%d:%d ago\n", ns_to_timespec64(diff).tv_sec,
																	t_n.msec, t_n.usec, t_n.nsec);
}

CLASS_ATTR_CREATE(REL_ATTR_NAME, 0444, rel_show, NULL);


///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

int __init mod_init(void)
{
	int res = 0;

	timer_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(timer_class))
		pr_info("bad class create\n");

	res = class_create_file(timer_class, &CLASS_ATTR(ABS_ATTR_NAME));
	if (IS_ERR_VALUE(res)) {
		pr_err("couldnt create 'absolute file':(\n");

		goto err;
	}

	res = class_create_file(timer_class, &CLASS_ATTR(REL_ATTR_NAME));
	if (IS_ERR_VALUE(res)) {
		pr_err("couldnt create 'relative' file:(\n");

		goto err;
	}

	pr_info("'task_05' module initialized\n");
err:
	return res;
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

void mod_cleanup(void)
{
	class_remove_file(timer_class, &CLASS_ATTR(REL_ATTR_NAME));
	class_remove_file(timer_class, &CLASS_ATTR(ABS_ATTR_NAME));
	class_destroy(timer_class);
}

module_init(mod_init);
module_exit(mod_cleanup);
