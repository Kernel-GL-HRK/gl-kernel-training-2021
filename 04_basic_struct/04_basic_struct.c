// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>

#define CLASS_NAME    "module_04"
#define SUCCESS       0

MODULE_DESCRIPTION("Implement module which includes linked list structure");
MODULE_AUTHOR("Ihor Stepukh");
MODULE_LICENSE("Dual MIT/GPL");

struct node {
	struct list_head my_list;
	char *my_data;
};

static struct class *hello_class;

LIST_HEAD(hello_list);

static ssize_t write_data(struct class *class, struct class_attribute *attr,
				char *buf)
{
	struct node *list_node_ptr = NULL;

	list_for_each_entry(list_node_ptr, &hello_list, my_list) {
		strcat(buf, list_node_ptr->my_data);
	}
	strcat(buf, "\n");

	return strlen(buf);
}

static ssize_t read_data(struct class *class, struct class_attribute *attr,
				const char *buf, size_t count)
{
	struct node *node;
	size_t len = strnlen(buf, count);

	if (len != count) {
		pr_err("Incorrect length\n");
		return -EIO;
	}

	node = kmalloc(sizeof(*node), GFP_KERNEL);
	if (node == NULL)
		return -ENOMEM;

	node->my_data = kstrdup(buf, GFP_KERNEL);
	if (node->my_data == NULL)
		return -EAGAIN;

	list_add_tail(&(node->my_list), &hello_list);

	return count;
}

struct class_attribute hello_class_attr = {
	.attr = { .name = "hello_list", .mode = 0666 },
	.show	= write_data,
	.store	= read_data
};

static int __init module_04_init(void)
{
	int ret = 0;

	hello_class = class_create(THIS_MODULE, CLASS_NAME);
	if (hello_class == NULL) {
		pr_err("Unable to create class %s\n", CLASS_NAME);
		return -ENOMEM;
	}

	ret = class_create_file(hello_class, &hello_class_attr);
	if (ret != 0) {
		pr_err("Unable to create file in class %s\n", CLASS_NAME);
		class_destroy(hello_class);
		return ret;
	}

	pr_info("Module %s starts\n", THIS_MODULE->name);

	return SUCCESS;
}

static void __exit module_04_exit(void)
{
	struct node *cur = NULL;
	struct node *tmp;

	list_for_each_entry_safe(cur, tmp, &hello_list, my_list) {
		list_del(&cur->my_list);
		kfree(cur->my_data);
		kfree(cur);
	}

	class_remove_file(hello_class, &hello_class_attr);
	class_destroy(hello_class);

	pr_info("Module %s finished\n", THIS_MODULE->name);
}

module_init(module_04_init);
module_exit(module_04_exit);
