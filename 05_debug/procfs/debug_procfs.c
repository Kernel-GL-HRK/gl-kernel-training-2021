// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("Debug procfs to uppercase converter");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

static struct proc_dir_entry *dprocfs_dir;
static struct proc_dir_entry *dprocfs_uppercase;
static struct proc_dir_entry *dprocfs_statistics;

static char s[PAGE_SIZE] = "string empty!\n";
static int total_calls_show;
static int total_calls_write;
static int total_char_processed;
static int total_char_converted;

static ssize_t write_uppercase(struct file *file, const char __user *pbuf,
			       size_t count, loff_t *ppos)
{
	int s_offset = 0;
	ssize_t msg_size = 0;

	total_calls_write++;
	msg_size = simple_write_to_buffer(s, sizeof(s), ppos, pbuf, count);

	if (msg_size < 0) {
		pr_err("Can't copy your string\n");
		return msg_size;
	}

	s[msg_size] = '\0';

	pr_info("Before convertion = %s", s);
	for (; s[s_offset] != '\0'; s_offset++) {
		if (s[s_offset] >= 'a' && s[s_offset] <= 'z') {
			s[s_offset] = s[s_offset] - ('a' - 'A');
			total_char_converted++;
		}
	}

	pr_info("After convertion = %s", s);

	total_char_processed += msg_size-1;

	return msg_size;
}

static ssize_t read_uppercase(struct file *file, char __user *pbuf,
			      size_t count, loff_t *ppos)
{
	total_calls_show++;
	return simple_read_from_buffer(pbuf, count, ppos, s, strlen(s));
}

static ssize_t read_statistics(struct file *file, char __user *pbuf,
			       size_t count, loff_t *ppos)
{
	int initial_offset = strlen(s);
	int buf_offset = initial_offset + 1;

	buf_offset += sprintf(s + buf_offset, "%s",
			      "Debug_procfs statistics:\n");
	buf_offset += sprintf(s + buf_offset, "%s",
			      "Total calls show string = ");
	buf_offset += sprintf(s + buf_offset, "%d\n", total_calls_show);
	buf_offset += sprintf(s + buf_offset, "%s",
			      "Total calls write string = ");
	buf_offset += sprintf(s + buf_offset, "%d\n", total_calls_write);
	buf_offset += sprintf(s + buf_offset, "%s",
			      "Total characters processed = ");
	buf_offset += sprintf(s + buf_offset, "%d\n", total_char_processed);
	buf_offset += sprintf(s + buf_offset, "%s",
			      "Total characters converted = ");
	buf_offset += sprintf(s + buf_offset, "%d\n", total_char_converted);

	return simple_read_from_buffer(pbuf, count, ppos,
				       s+initial_offset+1,
				       strlen(s+initial_offset+1));
}

static const struct file_operations fops_dprocfs_uppercase = {
	.owner = THIS_MODULE,
	.read = read_uppercase,
	.write = write_uppercase
};

static const struct file_operations fops_dprocfs_statistics = {
	.owner = THIS_MODULE,
	.read = read_statistics
};

static int dprocfs_init(void)
{
	dprocfs_dir = proc_mkdir("dprocfs_dir", NULL);
	if (dprocfs_dir == NULL) {
		pr_err("mymodule: error creating procfs dir\n");
		return -ENOMEM;
	}

	dprocfs_uppercase = proc_create("dprocfs_uppercase", 0644,
					dprocfs_dir, &fops_dprocfs_uppercase);
	if (dprocfs_uppercase == NULL) {
		pr_err("mymodule: error creating procfs file uppercase\n");
		goto remove_dir;
	}

	dprocfs_statistics = proc_create("dprocfs_statistics", 0444,
					dprocfs_dir, &fops_dprocfs_statistics);
	if (dprocfs_uppercase == NULL) {
		pr_err("mymodule: error creating procfs file uppercase\n");
		goto remove_uppercase;
	}

	pr_info("mymodule: module loaded\n");
	return 0;

remove_uppercase:
	proc_remove(dprocfs_uppercase);
remove_dir:
	proc_remove(dprocfs_dir);
	return -ENOMEM;
}

static void dprocfs_exit(void)
{
	proc_remove(dprocfs_statistics);
	proc_remove(dprocfs_uppercase);
	proc_remove(dprocfs_dir);
	pr_info("mymodule: module exited\n");
}

module_init(dprocfs_init);
module_exit(dprocfs_exit);

