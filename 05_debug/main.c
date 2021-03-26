// SPDX-License-Identifier: MIT and GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

#include <linux/sysfs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/ctype.h>

static struct kobject *hello;
static size_t try_sysfs_read;
static size_t try_procfs_read;

static LIST_HEAD(procfs_list);
static LIST_HEAD(sysfs_list);

struct hello_node {
	struct list_head head;
	char *string_fs;
};

static ssize_t rw_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	struct hello_node *item = NULL;
	int offset = 0;
	int state = 0;

	try_sysfs_read++;
	list_for_each_entry(item, &sysfs_list, head) {
		state = snprintf(buf + offset, PAGE_SIZE - offset, "%s",
				item->string_fs);
		if (state < 0) {
			pr_err("mymodule : error reading\n");
			return state;
		}
		pr_info("mymodule : success reading\n");
		offset += state;
	}
	return offset;
}

static ssize_t rw_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct hello_node *item = kzalloc(sizeof(*item), GFP_KERNEL);
	size_t i = 0;

	if (strlen(buf) != count)
		return -EINVAL;
	item->string_fs = kmalloc_array(count, sizeof(char), GFP_KERNEL);
	while (i < count) {
		item->string_fs[i] = tolower(buf[i]);
		i++;
	}
	item->string_fs[i] = '\0';
	list_add(&item->head, &sysfs_list);
	pr_info("mymodule: new item in list sysfs %s", buf);
	return count;
}

static struct kobj_attribute attr =
	__ATTR(list, 0660, rw_show, rw_store);
static int mysysfs_init(void)
{
	int state = 0;

	hello = kobject_create_and_add("kobject_list", kernel_kobj);
	if (hello == NULL) {
		pr_info("impossible to create Kobject!\n");
		return -EINVAL;
	}
	state = sysfs_create_file(hello, &attr.attr);
	if (state) {
		kobject_put(hello);
		pr_info("impossible to create File!\n");
		return state;
	}
	return state;
}

static ssize_t rw_write(struct file *file, const char __user *pbuf,
		size_t count, loff_t *ppos)
{
	struct hello_node *item = kzalloc(sizeof(*item), GFP_KERNEL);
	size_t i = 0;

	item->string_fs = kmalloc_array(count, sizeof(char), GFP_KERNEL);
	copy_from_user(item->string_fs, pbuf, count);
	pr_info("mymodule: new item in list procfs %s\n", item->string_fs);
	while (i < count) {
		item->string_fs[i] = toupper(item->string_fs[i]);
		i++;
	}
	list_add(&item->head, &procfs_list);
	return count;
}

static ssize_t l = PAGE_SIZE + 1;

static ssize_t rw_read(struct file *file, char __user *pbuf, size_t count,
		loff_t *ppos)
{
	size_t min = 0;
	struct hello_node *item = NULL;
	int offset = 0;
	int state = 0;

	min = min_t(ssize_t, count, l);
	try_procfs_read++;
	if (min) {
		l = 0;
		list_for_each_entry(item, &procfs_list, head) {
			state = snprintf(pbuf + offset, PAGE_SIZE - offset,
					"%s", item->string_fs);
			if (state < 0) {
				pr_err("mymodule : error reading\n");
				return state;
			}
			pr_info("mymodule : success reading\n");
			offset += state;
		}
	}
	return offset;
}

static struct proc_dir_entry *entry;
static struct proc_dir_entry *dir;
static const struct file_operations oper = {
	.owner = THIS_MODULE,
	.read = rw_read,
	.write = rw_write,
};

static int myprocfs_init(void)
{
	pr_info("INIT procfs\n");
	dir = proc_mkdir("procfs_list", NULL);
	if (dir == NULL) {
		pr_err("mymodule: error creating procfs directory\n");
		kobject_put(hello);
		return -ENOMEM;
	}
	entry = proc_create("list", 0666, dir, &oper);
	if (entry == NULL) {
		pr_err("mymodule: error creating procfs entry\n");
		remove_proc_entry("procfs_list", NULL);
		kobject_put(hello);
		return -ENOMEM;
	}
	return 0;
}

static int mymodule_init(void)
{
	mysysfs_init();
	myprocfs_init();
	pr_info("Module initialized successfully\n");
	return 0;
}

static void mymodule_exit(void)
{
	struct hello_node *item = NULL;
	struct hello_node *tmp = NULL;

	pr_info("Total cat call for /sys/kernel/kobject_list/list: %d",
			try_sysfs_read);
	pr_info("Total cat call for /proc/procfs_list/list: %d",
			try_procfs_read - 2);
	list_for_each_entry_safe(item, tmp, &sysfs_list, head) {
		list_del(&item->head);
		kfree(item->string_fs);
		kfree(item);
	}
	pr_info("Task 05. Done!\n");
	kobject_put(hello);
	proc_remove(dir);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Vladyslav Andrishko <v.andrishko.v333@gmail.com>");
MODULE_DESCRIPTION("Kernel module example with list structure");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

