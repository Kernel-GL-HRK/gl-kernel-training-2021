// SPDX-License-Identifier: GPL-2.0
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/ktime.h>

MODULE_DESCRIPTION("Basic module demo: work with time");
MODULE_AUTHOR("Anton KOtsiubailo");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define LEN_MSG 160
#define CLASS_ATTR(_name, _mode, _show, _store)				\
struct class_attribute class_attr_##_name =				\
__ATTR(_name, _mode, _show, _store)

static char buf_msg[LEN_MSG + 1] = "Hello from module!\n";
static ktime_t currtime;
static ktime_t pasttime;
static ktime_t prev;

static ssize_t x_show(struct class *class, struct class_attribute *attr,
			char *buf)
{
	struct timespec time;

	currtime = ktime_get();
	time = ktime_to_timespec(currtime - pasttime);
	strcpy(buf, buf_msg);
	pr_info("read %ld\n", (long)strlen(buf));
	pr_info("time: %d.%09ld sec\n", time.tv_sec, time.tv_nsec);
	time = ktime_to_timespec(prev);
	pr_info("previous time: %d.%09ld sec\n", time.tv_sec, time.tv_nsec);
	prev = currtime - pasttime;
	pasttime = currtime;

	return strlen(buf);
}


CLASS_ATTR(xxx, 0644, &x_show, NULL);

static struct class *x_class;

int __init x_init(void)
{
	int res;

	x_class = class_create(THIS_MODULE, "x-class");
	if (IS_ERR(x_class)) {
		pr_err("bad class create\n");
	}
	res = class_create_file(x_class, &class_attr_xxx);
	pr_info("'xxx' module initialized\n");

	return res;
}

void __exit x_cleanup(void)
{
	class_remove_file(x_class, &class_attr_xxx);
	class_destroy(x_class);
}

module_init(x_init);
module_exit(x_cleanup);
