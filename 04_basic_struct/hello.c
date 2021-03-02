// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/slab.h> //kmalloc
#include <linux/string.h>

MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("Linux kernel data structure");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

static LIST_HEAD(hello_list);
struct hello_node {
	struct list_head list;
	char *data;
};


struct hello_node *list_create_node(const char *s, size_t count)
{
	struct hello_node *node = kmalloc(sizeof(*node),
					GFP_KERNEL);

	if (node == NULL)
		pr_err("Element hasn`t created. Not enough memory for list entry\n");

	node->data = kmalloc_array(count, sizeof(char), GFP_KERNEL);

	strcpy(node->data, s);

	return node;
}

void list_delete_node(struct hello_node *a_node)
{
	if (a_node == NULL)
		return;

	kfree(a_node->data);
	kfree(a_node);
}

int list_print(struct list_head *print_list, char *buf)
{
	const char *arrow = " -> ";
	int buf_offset = 0;
	int current_offset = 0;
	struct hello_node *current_node = NULL;

	if (print_list == NULL) {
		pr_err("Can't print NULL list\n");
		return 0;
	}

	if (list_empty(print_list) == 1) {
		return sprintf(buf+buf_offset, "%s\n",
			"List empty!\n"
			"USAGE:\n"
			"at_<string> - add to tail\n"
			"ah_<string> - add to head\n"
			"rt - remove from tail\n"
			"rh - remove from head");
	}

	current_offset = sprintf(buf, "%s", "List: ");
	if (current_offset < 0)
		return buf_offset;

	buf_offset += current_offset;

	list_for_each_entry(current_node, &hello_list, list) {

		current_offset = sprintf(buf+buf_offset,
		"%s", current_node->data)-1;
		if (current_offset < 0)
			return buf_offset;

		buf_offset += current_offset;

		current_offset = sprintf(buf+buf_offset, "%s", arrow);
		if (current_offset < 0)
			return buf_offset;

		buf_offset += current_offset;
	}

	return buf_offset+sprintf(buf+buf_offset, "%s", "\n");
}

void list_erase(struct list_head *erase_list)
{
	struct hello_node *current_node = NULL;
	struct hello_node *tmp = NULL;

	if (erase_list == NULL)
		return;

	if (list_empty(erase_list) == 1)
		return;

	list_for_each_entry_safe(current_node, tmp, erase_list, list) {
		list_del(&current_node->list);
		list_delete_node(current_node);
	}
}


static struct kobject *hello_kobj;

static ssize_t show_list(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return list_print(&hello_list, buf);
}

static ssize_t store_list(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	struct hello_node *a_node = NULL;
	const int cmd_size = 2;
	char *cmd = NULL;

	//check on minimal cmd
	if (count < 3) {
		pr_err("Wrong cmd!\n");
		return count;
	}


	//extract cmd string
	cmd = kmalloc_array(cmd_size+1, sizeof(char), GFP_KERNEL);
	cmd[cmd_size] = '\0';
	strncpy(cmd, buf, cmd_size);

	//action for deleting element from list
	if (list_empty(&hello_list) != 1 && count == 3) {
		if (strcmp(cmd, "rt") == 0) {
			a_node = list_entry(hello_list.prev, struct hello_node,
									list);
			list_del(hello_list.prev);
			list_delete_node(a_node);
		} else if (strcmp(cmd, "rh") == 0) {
			a_node = list_entry(hello_list.next, struct hello_node,
									list);
			list_del(hello_list.next);
			list_delete_node(a_node);
		} else {
			pr_err("Wrong cmd!\n");
			goto exit;
		}
		goto exit;
	}

	//check condition for add cmd
	// 3 - '_' + one letter of data + \0
	if (count < cmd_size + 3 || buf[cmd_size] != '_') {
		pr_err("Wrong cmd!\n");
		goto exit;
	}

	if (strcmp(cmd, "at") == 0) {
		a_node = list_create_node(buf+cmd_size+1, count-cmd_size+1);
		list_add_tail(&a_node->list, &hello_list);
	} else if (strcmp(cmd, "ah") == 0) {
		a_node = list_create_node(buf+cmd_size+1, count-cmd_size+1);
		list_add(&a_node->list, &hello_list);
	} else {
		pr_err("Wrong cmd!\n");
	}

exit:
	kfree(cmd);
	return count;
}

static struct kobj_attribute hello_attr_list = {
	.attr = { .name = "hello_list", .mode = 0644 },
	.show	= show_list,
	.store	= store_list
};

static struct kobj_attribute hello_attr_tree = {
	.attr = { .name = "hello_another_list", .mode = 0644 },
	.show	= show_list,
	.store	= store_list
};

static struct kobj_attribute hello_attr_hash = {
	.attr = { .name = "hello_one_more_list", .mode = 0644 },
	.show	= show_list,
	.store	= store_list
};

static struct attribute *hello_attrs[] = {
	&hello_attr_list.attr,
	&hello_attr_tree.attr,
	&hello_attr_hash.attr,
	NULL
};

// I planned also do tree and hash but did not calculate the time
static struct attribute_group hello_attr_group = {
	.attrs = hello_attrs,
};



static int __init example_init(void)
{
	int retval;

	hello_kobj = kobject_create_and_add("hello_dir", kernel_kobj);
	if (hello_kobj == NULL) {
		pr_err("Error while creating a kobj directory\n");
		return -ENOMEM;
	}

	retval = sysfs_create_group(hello_kobj, &hello_attr_group);
	if (retval) {
		pr_err("Error while creating a group\n");
		kobject_put(hello_kobj);
		return retval;
	}

	pr_info("module loaded\n");
	return 0;
}

static void __exit example_exit(void)
{
	list_erase(&hello_list);
	sysfs_remove_group(hello_kobj, &hello_attr_group);
	kobject_put(hello_kobj);
	pr_info("module exited\n");
}


module_init(example_init);
module_exit(example_exit);
