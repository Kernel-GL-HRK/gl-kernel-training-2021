// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause


#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/printk.h>

MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Basic structure in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

#define DIR_NAME        "hello"
#define ENT_DATA_NAME	"list"

static LIST_HEAD(hello_list);

struct hello {
	struct list_head list;
	char *data;
};

static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	int size = 0;
	struct hello *node = NULL;

	list_for_each_entry(node, &hello_list, list) {
		size += scnprintf(buf + size, (PAGE_SIZE - size),
				"%s", node->data);
	}

	return size;
}

static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	struct hello *node = NULL;

	node = kmalloc(sizeof(struct hello), GFP_KERNEL);
	if (node == NULL)
		return -ENOMEM;

	node->data = kmalloc(sizeof(char) * count + 1, GFP_KERNEL);
	if (!node) {
		kfree(node);
		return -ENOMEM;
	}

	strncpy(node->data, buf, count);
	node->data[count] = '\0';

	list_add_tail(&node->list, &hello_list);
	pr_info("Added item list: %s", node->data);

	return count;
}

static struct kobject *hello_kobj;
struct kobj_attribute hello_kobj_attr = {
	.attr = { .name = ENT_DATA_NAME, .mode = 0664 },
	.show   = hello_show,
	.store  = hello_store
};

static int __init data_list_init(void)
{
	int res;

	hello_kobj = kobject_create_and_add(DIR_NAME, kernel_kobj);
	if (hello_kobj == NULL) {
		pr_err("data_list: error! cannot create sysfs kobject\n");
		return -ENOMEM;
	}

	res = sysfs_create_file(hello_kobj, &hello_kobj_attr.attr);
	if (res) {
		pr_err("error! cannot create sysfs class attribute\n");
		kobject_put(hello_kobj);

	}
	pr_info("=== data_list module loaded ===\n");
	return 0;
}

static void __exit data_list_exit(void)
{
	struct hello *node = NULL;
	struct hello *tmp = NULL;

	list_for_each_entry_safe(node, tmp, &hello_list, list) {
		list_del(&node->list);
		kfree(node->data);
		kfree(node);
	}
	kobject_put(hello_kobj);
	pr_info("--- data_list module unloaded ---\n");
}

module_init(data_list_init);
module_exit(data_list_exit);
