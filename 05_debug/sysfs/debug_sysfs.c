// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/device.h>
#include <linux/string.h>

MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("Debug sysfs to lowercase converter");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

static struct class *class_dsysfs;
static struct device *device_dsysfs;
static dev_t dev_t_dsysfs;

static char s[PAGE_SIZE] = "string empty!";
static int total_calls_show;
static int total_calls_write;
static int total_char_processed;
static int total_char_converted;


ssize_t show_lowercase(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	total_calls_show++;
	return sprintf(buf, "%s\n", s);
}

ssize_t store_lowercase(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	int s_offset = 0;
	int retval = 0;

	total_calls_write++;

	retval = sscanf(buf, "%s\n", s);

	if (retval < 0) {
		pr_err("Can't copy your string\n");
		return count;
	}

	pr_info("Before convertion = %s\n", s);

	for (; s[s_offset] != '\0'; s_offset++) {
		if (s[s_offset] > 'A' && s[s_offset] < 'Z') {
			s[s_offset] += ('a' - 'A');
			total_char_converted++;
		}
	}

	total_char_processed += count-1;

	pr_info("After convertion = %s\n", s);
	return count;
}


ssize_t show_statistics(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	return sprintf(buf, "Debug_sysfs statistics:\n"
				"Total calls show string = %i\n"
				"Total calls write string =  %i\n"
				"Total characters processed = %i\n"
				"Total characters converted = %i\n",
				total_calls_show, total_calls_write,
				total_char_processed, total_char_converted);
}



struct device_attribute  device_attr_lowercase = {
	.attr = { .name = "dsysfs_lowercase", .mode = 0644 },
	.show	= show_lowercase,
	.store	= store_lowercase
};

struct device_attribute  device_attr_statistics = {
	.attr = { .name = "dsysfs_statistics", .mode = 0444 },
	.show	= show_statistics,
};


static struct attribute *attrs_dsysfs[] = {
	&device_attr_lowercase.attr,
	&device_attr_statistics.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

struct attribute_group device_attr_group_dsysfs = {
	.attrs = attrs_dsysfs
};

static int __init dsysfs_init(void)
{
	int must_check = 0;

	class_dsysfs = class_create(THIS_MODULE, "dsysfs_dir");
	if (class_dsysfs == NULL) {
		pr_err("Error while creating a class\n");
		return -ENOMEM;
	}

	device_dsysfs = device_create(class_dsysfs, NULL, dev_t_dsysfs,
				      NULL, "dsysfs_device");
	if (device_dsysfs == NULL) {
		pr_err("Error while creating a device\n");
		class_destroy(class_dsysfs);
		return -ENOMEM;
	}

	must_check = sysfs_create_group(&(device_dsysfs->kobj),
					&device_attr_group_dsysfs);
	if (must_check) {
		pr_err("Error while creating a group of files\n");
		device_destroy(class_dsysfs, dev_t_dsysfs);
		class_destroy(class_dsysfs);
		return must_check;
	}

	pr_info("module loaded\n");
	return 0;
}

static void __exit dsysfs_exit(void)
{
	sysfs_remove_group(&(device_dsysfs->kobj), &device_attr_group_dsysfs);
	device_destroy(class_dsysfs, dev_t_dsysfs);
	class_destroy(class_dsysfs);
	pr_info("module exited\n");
}

module_init(dsysfs_init);
module_exit(dsysfs_exit);

