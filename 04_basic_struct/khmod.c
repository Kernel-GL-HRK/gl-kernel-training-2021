// SPDX-License-Identifier: MIT and GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>

MODULE_DESCRIPTION("Sysfs llist demo module");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

struct list_node {
	struct list_head list;
	char *data;
};

static struct class *attr_class = 0;

struct list_node first = {
	.data = "data1",
	.list = LIST_HEAD_INIT(first.list)
};

struct list_node second = {
	.data = "data2",
	.list = LIST_HEAD_INIT(second.list)
};

LIST_HEAD(mylist);

static ssize_t rw_show(struct class *class, struct class_attribute *attr, char *buf)
{
	struct list_node *list_node_ptr = NULL;
	
	list_for_each_entry_reverse(list_node_ptr, &mylist, list) {
		strcat(buf, list_node_ptr->data);
		strcat(buf, " ");
	}
	strcat(buf, "\n");

	return strlen(buf);
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	return count;
}

struct class_attribute class_attr_rw = {
	.attr = { .name = "rw", .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

static int __init firstmod_init(void)
{
	int ret = 0;
	attr_class = class_create(THIS_MODULE, "kharchuk");
	
	if (attr_class == NULL) {
		pr_err("%s: error creating sysfs class\n", THIS_MODULE->name);
		return -ENOMEM;
	}
	ret = class_create_file(attr_class, &class_attr_rw);
	
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n", THIS_MODULE->name);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("%s: loaded!\n", THIS_MODULE->name);
	
	list_add(&first.list, &mylist);
	list_add(&second.list, &mylist);
	
	return 0;
}

static void __exit firstmod_exit(void)
{
	class_remove_file(attr_class, &class_attr_rw);
	class_destroy(attr_class);
	
	pr_info("%s: removed!\n", THIS_MODULE->name);
}

module_init(firstmod_init);
module_exit(firstmod_exit);

