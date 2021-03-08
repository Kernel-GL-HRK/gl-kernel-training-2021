// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/printk.h>

MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Debug info in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

#define DIR_NAME	"debug_info_dir"
#define ENT_UPPER_NAME	"uppercase"
#define ENT_STAT_NAME	"info"

static ssize_t procfs_read(struct file *file, char __user *pbuf,
				size_t count, loff_t *ppos);
static ssize_t procfs_write(struct file *file, const char __user *pbuf,
				size_t count, loff_t *ppos);
static ssize_t procfs_read_info(struct file *file, char __user *pbuf,
				size_t count, loff_t *ppos);

static char buffer[PAGE_SIZE];
static int num_calls_read;
static int num_calls_write;
static int num_char_processed;
static int num_char_converted;

static struct file_operations proc_ops_uppercase = {
	.owner	= THIS_MODULE,
	.read	= procfs_read,
	.write	= procfs_write
};

static struct file_operations proc_ops_info = {
	.owner	= THIS_MODULE,
	.read	= procfs_read_info
};

static struct proc_dir_entry *ent;
static struct proc_dir_entry *upper;
static struct proc_dir_entry *info;


static int test_procfs_init(void)
{
	ent = proc_mkdir(DIR_NAME, NULL);
	if (ent == NULL) {
		pr_err("%s: error! cannot create dir on procfs\n", __FILE__);
		return -ENOMEM;
	}

	upper = proc_create(ENT_UPPER_NAME, 0666,
			ent, &proc_ops_uppercase);
	if (upper == NULL) {
		pr_err("debug_module: error! cannot create file uppercase\n");
		goto rm_dir;
	}

	info = proc_create(ENT_STAT_NAME, 0444,
			ent, &proc_ops_info);
	if (info == NULL) {
		pr_err("debug_module: error! cannot create file info\n");
		goto rm_upper;
	}
	pr_info("=== debug_module loaded ===\n");
	return 0;

rm_upper:
	proc_remove(upper);
rm_dir:
	proc_remove(ent);
	return -ENOMEM;
}

static void test_procfs_exit(void)
{
	proc_remove(ent);
	pr_info("--- debug_module removed ---\n");
}

ssize_t procfs_read(struct file *file, char __user *pbuf,
				size_t count, loff_t *ppos)
{
	num_calls_read++;
	return simple_read_from_buffer(pbuf, count, ppos,
					buffer, strlen(buffer));
}

ssize_t procfs_write(struct file *file, const char __user *pbuf,
				size_t count, loff_t *ppos)
{
	ssize_t str_size = 0;
	int i = 0;

	num_calls_write++;
	str_size = simple_write_to_buffer(buffer, sizeof(buffer),
						ppos, pbuf, count);

	if (str_size < 0) {
		pr_err("debug_module: error! can't get user data.\n");
		return str_size;
	}
	pr_info("passed data: %s\n", buffer);

	while (buffer[i] != 0) {
		if (buffer[i] >= 'a' && buffer[i] <= 'z') {
			buffer[i] = buffer[i] - 32;
			num_char_converted++;
		}
		++i;
	}

	pr_info("data in Uppercase: %s", buffer);
	num_char_processed += str_size - 1;

	return str_size;
}

ssize_t procfs_read_info(struct file *file, char __user *pbuf,
				size_t count, loff_t *ppos)
{
	char info_buffer[256];
	int offset;

	offset = sprintf(info_buffer, "Uppercase conversion stat info:\n");
	offset += sprintf(info_buffer + offset,
			"Total calls to read string: %d\n", num_calls_read);
	offset += sprintf(info_buffer + offset,
			"Total calls to write string:%d\n", num_calls_write);
	offset += sprintf(info_buffer + offset,
			"Total characters processed:%d\n", num_char_processed);
	offset += sprintf(info_buffer + offset,
			"Total characters converted:%d\n", num_char_converted);

	return simple_read_from_buffer(pbuf, count, ppos,
					info_buffer, strlen(info_buffer));
}

module_init(test_procfs_init);
module_exit(test_procfs_exit);
