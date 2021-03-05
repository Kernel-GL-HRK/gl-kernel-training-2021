// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define BUF_SIZE 4096
#define MODULE_NAME "basic_struct"
#define STAMP_STRING "List entry:\n'%s'\n"

struct my_data {
	struct list_head list;
	char *str;
};

static LIST_HEAD(my_list_head);

static ssize_t list_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	struct my_data *ptr;
	size_t size = 0;

	list_for_each_entry(ptr, &my_list_head, list) {
		size += snprintf(buf + size, BUF_SIZE, STAMP_STRING, ptr->str);
	}

	return size;
}

static ssize_t list_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	struct my_data *ptr = kmalloc(sizeof(struct my_data), GFP_KERNEL);

	if (!ptr)
		return -ENOMEM;

	ptr->str = kmalloc(sizeof(char) * count + 1, GFP_KERNEL);

	if (!ptr->str) {
		kzfree(ptr);
		return -ENOMEM;
	}

	/* For copy '\0' character add 1 to size */
	memcpy(ptr->str, buf, count + 1);

	list_add(&(ptr->list), &my_list_head);

	return count;
}

static struct kobj_attribute list_attribute =
	__ATTR(list, 0644, list_show, list_store);

static struct attribute *attrs[] = {
	&list_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *hello_obj;

static int __init basic_struct_init(void)
{
	int ret;

	/* Create kobject */
	hello_obj = kobject_create_and_add("hello", kernel_kobj);
	if (!hello_obj) {
		pr_err(MODULE_NAME ": error creating sysfs object\n");
		return -ENOMEM;
	}

	/* Create the files associated with this kobject */
	ret = sysfs_create_group(hello_obj, &attr_group);
	if (ret) {
		pr_err(MODULE_NAME ": error creating sysfs attribute\n");
		kobject_put(hello_obj);
		return ret;
	}

	pr_info(MODULE_NAME ": init success\n");
	return 0;
}

static void __exit basic_struct_exit(void)
{
	struct my_data *ptr, *tmp;

	kobject_put(hello_obj);
	list_for_each_entry_safe(ptr, tmp, &my_list_head, list) {
		pr_info(MODULE_NAME ": remove entry with string - '%s'",
				ptr->str);
		kzfree(ptr->str);
		kzfree(ptr);
	}

	pr_info(MODULE_NAME ": module release!\n");
}

module_init(basic_struct_init);
module_exit(basic_struct_exit);
