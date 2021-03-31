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

static struct kobject *sys_data;

static LIST_HEAD(procfs_list);
static LIST_HEAD(sysfs_list);

size_t try_sysfs_read;
size_t sysfs_count;
size_t try_procfs_read;
size_t procfs_count;
size_t pconv_ch;
size_t sconv_ch;

struct data_node {
	struct list_head head;
	char *string_fs;
};


static ssize_t stat_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	int offset = 0;
	int state = 0;

	state = snprintf(buf + offset, PAGE_SIZE - offset,
			"\nUsing 'cat' for sysfs: %d\nTotal read bytes: %d\nTotal converted character: %d\n\n",
			try_sysfs_read, sysfs_count, sconv_ch);
	if (state < 0) {
		pr_err("stat : error reading\n");
		return state;
	}
	offset += state;
	return offset;
}

static ssize_t rw_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	struct data_node *item = NULL;
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
	struct data_node *item = kzalloc(sizeof(*item), GFP_KERNEL);
	size_t i = 0;

	sysfs_count += count;
	if (strlen(buf) != count)
		return -EINVAL;
	item->string_fs = kmalloc_array(count, sizeof(char), GFP_KERNEL);
	while (i < count) {
		if (buf[i] != tolower(buf[i])) {
			item->string_fs[i] = tolower(buf[i]);
			sconv_ch++;
		} else
			item->string_fs[i] = buf[i];
		i++;
	}
	item->string_fs[i] = '\0';
	list_add(&item->head, &sysfs_list);
	pr_info("mymodule: new item in list sysfs %s", buf);
	return count;
}

static struct kobj_attribute attr =
	__ATTR(list, 0660, rw_show, rw_store);
static struct kobj_attribute stat =
	__ATTR(statistics, 0660, stat_show, NULL);

static int mysysfs_init(void)
{
	int state = 0;

	sys_data = kobject_create_and_add("kobject_list", kernel_kobj);
	if (sys_data == NULL) {
		pr_info("impossible to create Kobject!\n");
		return state;
	}
	state = sysfs_create_file(sys_data, &attr.attr);
	if (state)
		goto fail_sysfs;
	state = sysfs_create_file(sys_data, &stat.attr);
	if (state)
		goto fail_sysfs;
	return state;
fail_sysfs:
	pr_info("impossible to create File with data!\n");
	kobject_put(sys_data);
	return state;
}

static ssize_t rw_write(struct file *file, const char __user *pbuf,
		size_t count, loff_t *ppos)
{
	struct data_node *item = kzalloc(sizeof(*item), GFP_KERNEL);
	size_t i = 0;
	ssize_t res;
	char tmp;

	procfs_count += count;
	item->string_fs = kmalloc_array(count, sizeof(char), GFP_KERNEL);
	res = copy_from_user(item->string_fs, pbuf, count);
	pr_info("mymodule: new item in list procfs %s\n", item->string_fs);
	while (i < count) {
		tmp = item->string_fs[i];
		item->string_fs[i] = toupper(item->string_fs[i]);
		if (item->string_fs[i] != tmp)
			pconv_ch++;
		i++;
	}
	list_add(&item->head, &procfs_list);
	return count;
}

static ssize_t rw_read(struct file *file, char __user *pbuf, size_t count,
		loff_t *ppos)
{
	struct data_node *item = NULL;
	int offset = 0;
	int state = 0;

	if (*ppos != 0)
		return 0;
	try_procfs_read++;
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
	return offset;
}

static ssize_t stat_read(struct file *file, char __user *pbuf, size_t count,
		loff_t *ppos)
{
	int offset = 0;
	int state = 0;

	if (*ppos != 0)
		return 0;
	try_procfs_read++;
	state = snprintf(pbuf + offset, PAGE_SIZE - offset,
			"\nUsing 'cat' for procfs %d\nTotal read bytes: %d\nTotal converted character: %d\n\n",
			try_procfs_read, procfs_count, pconv_ch);
	if (state < 0) {
		pr_err("mymodule : error reading\n");
		return state;
	}
	offset += state;
	return offset;
}

static struct proc_dir_entry *entry;
static struct proc_dir_entry *dir;
static const struct file_operations oper = {
	.owner = THIS_MODULE,
	.read = rw_read,
	.write = rw_write,
};
static const struct file_operations sstat = {
	.owner = THIS_MODULE,
	.read = stat_read,
};

static int myprocfs_init(void)
{
	dir = proc_mkdir("procfs_list", NULL);
	if (dir == NULL) {
		pr_err("mymodule: error creating procfs directory\n");
		return -1;
	}
	entry = proc_create("list", 0666, dir, &oper);
	if (entry == NULL)
		goto fail_procfs;
	entry = proc_create("stat", 0444, dir, &sstat);
	if (entry == NULL)
		goto fail_procfs;
	return 0;
fail_procfs:
	pr_err("mymodule: error creating procfs entry\n");
	remove_proc_entry("procfs_list", NULL);
	return -1;
}

static int mymodule_init(void)
{
	if (mysysfs_init() == -1) {
		pr_err("Sysfs initialized error!\n");
		return -ENOMEM;
	} else if (myprocfs_init() == -1) {
		pr_err("Procfs initialized error!\n");
		return -ENOMEM;
	}
	pr_info("Module initialized successfully\n");
	return 0;
}

static void mymodule_exit(void)
{
	struct data_node *item = NULL;
	struct data_node *tmp = NULL;

	list_for_each_entry_safe(item, tmp, &sysfs_list, head) {
		list_del(&item->head);
		kfree(item->string_fs);
		kfree(item);
	}
	list_for_each_entry_safe(item, tmp, &procfs_list, head) {
		list_del(&item->head);
		kfree(item->string_fs);
		kfree(item);
	}
	pr_info("Task 05. Done!\n");
	kobject_put(sys_data);
	proc_remove(dir);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Vladyslav Andrishko <v.andrishko.v333@gmail.com>");
MODULE_DESCRIPTION("Kernel module example with list structure");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

