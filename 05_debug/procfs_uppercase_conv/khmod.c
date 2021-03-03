// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define DIR_NAME	"kharchuk"
#define ENT1_NAME	"rw"
#define ENT2_NAME	"stats"

MODULE_DESCRIPTION("Sysfs lowercase converter");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent1, *ent2;

static char msg[PAGE_SIZE] = {"DEF"};
static ssize_t msg_size;
static ssize_t main_write(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos) 
{
	ssize_t not_copied;
	not_copied = copy_from_user(msg, pbuf, count);
	msg_size = count - not_copied;

	return msg_size;
}

static ssize_t main_read(struct file *file, char __user *pbuf, size_t count, loff_t *ppos) 
{
	ssize_t num, not_copied;
	
	num = min_t(ssize_t, msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, msg, count);
		num -= not_copied;
	}
	msg_size = 0;
	return num;
}

static ssize_t stats_read(struct file *file, char __user *pbuf, size_t count, loff_t *ppos) 
{
	ssize_t num, not_copied;
	
	num = min_t(ssize_t, msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, msg, count);
		num -= not_copied;
	}
	msg_size = 0;
	return num;
}

static struct file_operations main_ops = {
	.owner = THIS_MODULE,
	.read = main_read,
	.write = main_write
};

static struct file_operations stats_ops = {
	.owner = THIS_MODULE,
	.read = stats_read,
};

static int __init firstmod_init(void)
{
	int ret = 0;
	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("%s: error creating procfs directory\n",
			THIS_MODULE->name);
		return -ENOMEM;
	}

	ent1 = proc_create(ENT1_NAME, 0666, dir, &main_ops);
	if (ent1 == NULL) {
		pr_err("%s: error creating procfs entry\n",
			THIS_MODULE->name);
		remove_proc_entry(DIR_NAME, dir);
		return -ENOMEM;
	}

	ent2 = proc_create(ENT2_NAME, 0444, dir, &stats_ops);
	if (ent2 == NULL) {
		pr_err("%s: error creating procfs entry\n",
			THIS_MODULE->name);
		remove_proc_entry(DIR_NAME, NULL);
		remove_proc_entry(ENT1_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("%s: loaded!\n", THIS_MODULE->name);

	return 0;
}

static void __exit firstmod_exit(void)
{
	proc_remove(dir);
	pr_info("%s: removed!\n", THIS_MODULE->name);
}

module_init(firstmod_init);
module_exit(firstmod_exit);

