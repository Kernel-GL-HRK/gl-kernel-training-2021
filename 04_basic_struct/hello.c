// SPDX-License-Identifier: GPL-3.0
/**/
#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/init.h>	// Macros used to mark up functions  __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kobject.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/sysfs.h>

static int value;

static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr,
	char *buf)
{
	pr_info("%s: value = %d\n", __func__, value);
	sprintf(buf, "%d\n", value);
	return strlen(buf);
}

static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr,
	const char *buf, size_t count)
{
	if (kstrtoint (buf, 0, &value))
		pr_err("Invalid value\n");
	return count;
}

static struct kobj_attribute list_attribute =
	__ATTR(list, 0664, hello_show, hello_store);

static struct kobject *hello_kobj;

static int __init hello_init(void)
{
	int res = 0;

	hello_kobj = kobject_create_and_add("hello", kernel_kobj);
	if (!hello_kobj)
		return -ENOMEM;
	res = sysfs_create_file(hello_kobj, &list_attribute.attr);
	if (res)
		kobject_put(hello_kobj);
	return res;
}

static void __exit hello_exit(void)
{
	kobject_put(hello_kobj);
	pr_info("module exited\n");
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Basic data structures module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
