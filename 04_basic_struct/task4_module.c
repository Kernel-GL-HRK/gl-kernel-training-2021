// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * Trouble kernel module, which returns
 * fail or success based on random rand_number
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/list.h>


MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Module show/store in linked list");
MODULE_LICENSE("Dual BSD/GPL");


static struct kobject *hello;

static LIST_HEAD(list);

struct hello_node {
	struct list_head head;
	char *name;
};


static ssize_t task4_module_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	struct hello_node *node = NULL;
	int offset = 0;
	int res = 0;

	list_for_each_entry(node, &list, head) {

		res = sprintf(buf + offset, "%s", node->name);
		if (res < 0)
			return res;

		offset += res;
	}

	return offset;
}


static ssize_t task4_module_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct hello_node *new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);

	new_node->name = kmalloc_array(count, sizeof(char), GFP_KERNEL);

	strcpy(new_node->name, buf);

	list_add(&new_node->head, &list);

	pr_info("You added %s", buf);

	return count;
}


static struct kobj_attribute hello_attribute =
	__ATTR(list, 0660, task4_module_show, task4_module_store);


static int __init task4_module_init(void)
{
	int ret_value = 0;

	pr_info("task4_module says: Hello!\n");

	hello = kobject_create_and_add("task4_folder", kernel_kobj);
	if (!hello)
		ret_value = 1;

	ret_value = sysfs_create_file(hello, &hello_attribute.attr);

	return ret_value;
}



static void __exit task4_module_exit(void)
{
	struct hello_node *node = NULL;
	struct hello_node *n = NULL;

	list_for_each_entry_safe(node, n, &list, head) {
		list_del(&node->head);
		kfree(node->name);
		kfree(node);
	}

	kobject_put(hello);

	pr_info("task4_module says: Bye!\n");
}

module_init(task4_module_init);
module_exit(task4_module_exit);
