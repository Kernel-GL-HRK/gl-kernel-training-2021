// SPDX-License-Identifier: GPL-2.0
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/version.h>

MODULE_DESCRIPTION("Basic module demo: sysfs");
MODULE_AUTHOR("Anton KOtsiubailo");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define LEN_MSG 160

#define CLASS_ATTR(_name, _mode, _show, _store)	\
struct class_attribute class_attr_##_name =	\
__ATTR(_name, _mode, _show, _store)

static char buf_msg[LEN_MSG + 1];
static uint32_t total_characters_processed;
static uint32_t characters_converted;
static uint32_t total_calls;

static ssize_t x_show(struct class *class, struct class_attribute *attr,
			char *buf)
{
	strcpy(buf, buf_msg);

	pr_info("read %ld\n", (long)strlen(buf));

	total_calls++;

	return strlen(buf);
}

static ssize_t x_store(struct class *class, struct class_attribute *attr,
			const char *buf, size_t count)
{
	int i;

	if (count > LEN_MSG) {
		pr_err("Entered string is too long\n");
		return -EINVAL;
	}

	strncpy(buf_msg, buf, count);
	for (i = 0; buf_msg[i] != '\0'; i++) {
		if (buf_msg[i] >= 'A' && buf_msg[i] <= 'Z') {
			buf_msg[i] = buf_msg[i] + 32;
			characters_converted++;
		}
	}

	total_characters_processed = count;

	pr_info("write %ld\n", (long)count);

	buf_msg[count] = '\0';

	return count;
}

static ssize_t total_characters_processed_show(struct class *class,
						struct class_attribute *attr,
						char *buf)
{
	return sprintf(buf, "%d\n", total_characters_processed);
}

static ssize_t characters_converted_show(struct class *class,
					struct class_attribute *attr,
					char *buf)
{
	return sprintf(buf, "%d\n", characters_converted);
}

static ssize_t total_calls_show(struct class *class,
				struct class_attribute *attr,
				char *buf)
{
	return sprintf(buf, "%d\n", total_calls);
}

CLASS_ATTR(xxx, 0644, &x_show, &x_store);

CLASS_ATTR(total_characters_processed, 0444,
		&total_characters_processed_show, NULL);

CLASS_ATTR(total_calls, 0444, &total_calls_show, NULL);

CLASS_ATTR(characters_converted, 0444, &characters_converted_show, NULL);

static struct class *x_class;

int __init x_init(void)
{
	int res;

	x_class = class_create(THIS_MODULE, "x-class");

	if (IS_ERR(x_class)) {
		pr_err("bad class create\n");
		return -EINVAL;
	}

	res = class_create_file(x_class, &class_attr_xxx);
	if (res != 0) {
		pr_err("bad xxx class file create\n");
		class_destroy(x_class);
		return -EINVAL;
	}

	res = class_create_file(x_class,
				&class_attr_total_characters_processed);
	if (res != 0) {
		pr_err("bad file 'total_characters_processed' create\n");
		return -EINVAL;
	}

	res = class_create_file(x_class, &class_attr_total_calls);
	if (res != 0) {
		pr_err("bad file 'total_calls' create\n");
		return -EINVAL;
	}

	res = class_create_file(x_class, &class_attr_characters_converted);
	if (res != 0) {
		pr_err("bad file 'characters_converted' create\n");
		return -EINVAL;
	}

	pr_info("'xxx' module initialized\n");
	return res;
}

void __exit x_cleanup(void)
{
	class_remove_file(x_class, &class_attr_characters_converted);
	class_remove_file(x_class, &class_attr_total_calls);
	class_remove_file(x_class, &class_attr_total_characters_processed);
	class_remove_file(x_class, &class_attr_xxx);
	class_destroy(x_class);
}

module_init(x_init);
module_exit(x_cleanup);
