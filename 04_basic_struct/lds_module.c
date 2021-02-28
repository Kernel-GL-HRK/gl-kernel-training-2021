// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

#include <linux/sysfs.h>
#include <linux/device.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("List data module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

struct list_data {
	struct list_head my_list;
	int data;
};

static LIST_HEAD(data_list_head);

static int list_entry_add(int data, struct list_head *head)
{
	struct list_data *entry;

	if (head == NULL) {
		pr_err("Linked list doesn't exist!\n");
		return -EINVAL;
	}

	entry = kmalloc(sizeof(struct list_data), GFP_KERNEL);

	if (entry == NULL)
		return -ENOMEM;

	entry->data = data;

	list_add(&entry->my_list, head);

	return 0;
}

static int list_print(struct list_head *head, char *buf)
{
	size_t list_size = 0;
	int res = 0;
	int cursor = 0;

	struct list_data *entry = NULL;

	if ((head == NULL) || (buf == NULL)) {
		pr_err("Linked list or output buffer doesn't exist!\n");
		return -EINVAL;
	}

	res += sprintf(&buf[cursor], "[head] => ");
	if (res > 0)
		cursor += res;
	else {
		pr_err("Failed to write output.\n");
		return -EIO;
	}

	list_for_each_entry(entry, head, my_list) {
		res = sprintf(&buf[cursor], "[%d] => ", entry->data);
		if (res > 0)
			cursor += res;
		else {
			pr_err("Failed to write output.\n");
			return -EIO;
		}
		list_size++;
	}

	res = sprintf(&buf[cursor], "[head], size = %d node(s)\n",
		list_size);

	if (res > 0)
		cursor += res;
	else {
		pr_err("Failed to write output.\n");
		return -EIO;
	}

	return 0;
}

static int list_free(struct list_head *head)
{
	struct list_data *entry = NULL;
	struct list_head *position = NULL,
					 *tmp = NULL;

	if (head == NULL) {
		pr_err("Linked list doesn't exist!\n");
		return -EINVAL;
	}

	list_for_each_safe(position, tmp, head) {
		entry = list_entry(position, struct list_data, my_list);
		list_del(position);
		kfree(entry);
	}
	return 0;
}

static ssize_t list_show(struct kobject *kobj, struct kobj_attribute *attr,
					   char *buf)
{
	list_print(&data_list_head, buf);
	return strlen(buf);
}

static ssize_t list_store(struct kobject *kobj, struct kobj_attribute *attr,
						const char *buf, size_t count)
{
	int res = 0;
	int tmp_value = 0;

	res = sscanf(buf, "%du", &tmp_value);
	if (res > 0)
		list_entry_add(tmp_value, &data_list_head);

	return count;
}

static struct kobject *hello_kobject;
struct kobj_attribute kobj_attr_list = {
	.attr = { .name = "list", .mode = 0666 },
	.show	= list_show,
	.store	= list_store
};

static int init_sysfs(void)
{
	int res;

	hello_kobject = kobject_create_and_add("listed_data", kernel_kobj);
	if (hello_kobject == NULL) {
		pr_err("bts_module: error creating kobject\n");
		return -ENOMEM;
	}

	res = sysfs_create_file(hello_kobject, &kobj_attr_list.attr);
	if (res != 0) {
		pr_err("bts_module: error creating sysfs file\n");
		kobject_del(hello_kobject);
		return res;
	}

	return 0;
}

static int __init lds_module_init(void)
{
	int res = 0;

	pr_info("lds_module: module starting\n");

	res = init_sysfs();
	if (res != 0)
		return res;

	return 0;
}

static void __exit lds_module_exit(void)
{
	if (hello_kobject != NULL) {
		sysfs_remove_file(hello_kobject, &kobj_attr_list.attr);
		kobject_del(hello_kobject);
	}

	list_free(&data_list_head);
	pr_info("lds_module: module exited\n");
}

module_init(lds_module_init);
module_exit(lds_module_exit);
