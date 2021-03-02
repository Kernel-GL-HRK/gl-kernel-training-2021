// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 04_basic_struct task
 *
 * Kernel loadable module which creates a kobject with representation 
 * in the sysfs virtual filesystem, kobject contains sysfs attribute 
 * with name 'list' which implements linked-list with read/write interface.
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/kobject.h>

static LIST_HEAD(hello_list_head);

struct hello {
	struct list_head head;
	char *data;
};

static void hello_free(void)
{
	struct hello *node = NULL;
	struct hello *tmp = NULL;

	list_for_each_entry_safe(node, tmp, &hello_list_head, head) {
		list_del(&node->head);
		kfree(node->data);
		kfree(node);
	}
}

static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char *_buf = buf;
	struct hello *node = NULL;

	list_for_each_entry(node, &hello_list_head, head) {
		/* add '->' for all non-first entries */
		if (_buf != buf) {
			sprintf(_buf, "->");
			_buf += 2; /* + strlen("->") */
		}

		sprintf(_buf, "[%s]", node->data);
		_buf += strlen(node->data) + 2; /* + strlen("[]") */
	}

	sprintf(_buf, "\n");
	_buf += 1;

	return _buf - buf;
}

static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	char *nl_ch;
	struct hello *node;

	node = kmalloc(sizeof(struct hello), GFP_KERNEL);
	if (!node)
		return -ENOMEM;

	/* get string length to newline character */
	nl_ch = strchr(buf, '\n');
	if (nl_ch)
		count = nl_ch - buf + 1; /* +1 for endline character */

	node->data = kmalloc(count, GFP_KERNEL);
	if (!node) {
		kfree(node);
		return -ENOMEM;
	}

	/* copy string to first new line character */
	strncpy(node->data, buf, count - 1);
	node->data[count - 1] = '\0';

	list_add_tail(&node->head, &hello_list_head);
	pr_info("New item in list: [%s]\n", node->data);

	return count;
}

/* sysfs attribute with name 'list' */
static struct kobj_attribute list_attribute =
	__ATTR(list, 0664, hello_show, hello_store);

static struct attribute *attrs[] = {
	&list_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

const struct attribute_group attr_group = {
	.attrs = attrs
};

static struct kobject *hello_kobj;

static int __init gl_basicstruct_init(void)
{
	int retval;

	pr_info("GL Kernel Training 2021\n");

	hello_kobj = kobject_create_and_add("hello_kobj", kernel_kobj);
	if (!hello_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(hello_kobj, &attr_group);
	if (retval)
		kobject_put(hello_kobj);

	return 0;
}

static void __exit gl_basicstruct_exit(void)
{
	hello_free();
	kobject_put(hello_kobj);
	pr_info("Exit from 04_basic_struct module\n");
}

module_init(gl_basicstruct_init);
module_exit(gl_basicstruct_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 04_basic_struct");
MODULE_LICENSE("GPL");
