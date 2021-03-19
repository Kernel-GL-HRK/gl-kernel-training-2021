// SPDX-License-Identifier: GPL-3.0
/**/
#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/init.h>	// Macros used to mark up functions  __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kobject.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/slab.h>

struct list_node {
	char *node;
	struct list_head head_list;
};

static LIST_HEAD(linkedlist);

static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr,
	char *buf)
{
	struct list_node *cur_node = NULL;
	int offset = 0;
	int res = 0;
	int cnt = 1;

	list_for_each_entry(cur_node, &linkedlist, head_list) {
		res = scnprintf(buf+offset, PAGE_SIZE-offset, "Node %d: %s",
			cnt++, cur_node->node);
		offset += res;
	}
	return offset;
}

static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr,
	const char *buf, size_t count)
{
	struct list_node *new_node =
		kmalloc(sizeof(struct list_node), GFP_KERNEL);

	if (!new_node)
		return -ENOMEM;
	new_node->node = kstrdup(buf, GFP_KERNEL);
	if (new_node->node == NULL)
		return -ENOMEM;
	list_add_tail(&new_node->head_list, &linkedlist);
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
	struct list_node *cur_node = NULL;
	struct list_node *tmp_node = NULL;

	list_for_each_entry_safe(cur_node, tmp_node, &linkedlist, head_list) {
		pr_info("remove node: %s", cur_node->node);
		list_del(&cur_node->head_list);
		kfree(cur_node->node);
		kfree(cur_node);
	}
	kobject_put(hello_kobj);
	pr_info("module exited\n");
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Basic data structures module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
