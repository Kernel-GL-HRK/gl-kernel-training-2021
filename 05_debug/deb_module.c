// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/vmalloc.h>

MODULE_AUTHOR("Ivan Kryvosheia");
MODULE_DESCRIPTION("Debug module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("1.0");

#define SYSFS_DIR_NAME		"sysfs_gl"
#define PROCFS_DIR_NAME		"procfs_gl"

static ssize_t sysfs_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count);

static ssize_t	proc_read(struct file *file, char __user *buf,
		size_t count, loff_t *ppos);
static ssize_t	proc_write(struct file *file, const char __user *ubuf,
		size_t count, loff_t *ppos);

static int init_sysfs_attr(void);
static void deinit_sysfs_attr(void);

static int init_procfs_attr(void);
static void deinit_procfs_attr(void);

const static struct kobj_attribute sysfs_attr = {
	.attr = { .name = "conv", .mode = 0666 },
	.show	= sysfs_show,
	.store	= sysfs_store
};

const static struct file_operations procfs_attr = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
};

struct my_obj {
	char str[PAGE_SIZE];
} procfs_obj;

struct my_obj2 {
	char *str;
} sysfs_obj;

//sysfs
struct kobject *kobj_inst;

//procfs
struct proc_dir_entry *proc_dir;

static int init_procfs_attr(void)
{
	/* create directory in /proc/ */
	proc_dir = proc_create(PROCFS_DIR_NAME, 0660, NULL, &procfs_attr);
	if (!proc_dir)
		return -ENOMEM;

	return 0;
}

static void deinit_procfs_attr(void)
{
	proc_remove(proc_dir);
}

ssize_t	proc_read(struct file *file, char __user *buf, size_t count,
		loff_t *ppos)
{
	pr_info("%s\n", procfs_obj.str);

	return 0;
}

ssize_t	proc_write(struct file *file, const char __user *ubuf,
		size_t count, loff_t *ppos)
{
	int i = 0;

	if (copy_from_user(procfs_obj.str, ubuf, count))
		return -EFAULT;

	for (; i < count; i++)
		procfs_obj.str[i] = toupper(procfs_obj.str[i]);

	return count;
}

static int init_sysfs_attr(void)
{
	int rc = 0;

	//create a directory into the /sys/kernel directory
	kobj_inst = kobject_create_and_add(SYSFS_DIR_NAME, kernel_kobj);

	if (!kobj_inst)
		return -1;

	rc = sysfs_create_file(kobj_inst, &sysfs_attr.attr);
	if (rc)
		return -1;

	return rc;
}

static void deinit_sysfs_attr(void)
{
	if (kobj_inst)
		kobject_put(kobj_inst);
}

static ssize_t sysfs_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	pr_info("%s", sysfs_obj.str);

	return 0;
}

static ssize_t sysfs_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int i = 0;

	kzfree(sysfs_obj.str);

	sysfs_obj.str = kstrdup(buf, GFP_KERNEL);
	if (sysfs_obj.str == NULL)
		return 0;

	for (; i < count; i++)
		sysfs_obj.str[i] = tolower(sysfs_obj.str[i]);

	return count;
}

static int __init deb_module_init(void)
{
	int rc = 0;

	rc = init_sysfs_attr();
	if (rc) {
		deinit_sysfs_attr();
		pr_err("sysfs not initialized\n");
		return -1;
	}

	rc = init_procfs_attr();
		if (rc) {
			deinit_procfs_attr();
			pr_err("procfs not initialized\n");
		return -1;
	}

	pr_info("ProCamp Debug Module inserted\n");
	return rc;
}

static void __exit deb_module_exit(void)
{
	deinit_sysfs_attr();

	deinit_procfs_attr();

	kzfree(sysfs_obj.str);

	pr_info("ProCamp Debug Module removed\n");
}

module_init(deb_module_init);
module_exit(deb_module_exit);
