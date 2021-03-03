// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>


MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Simple string uppercase converter using procfs");
MODULE_LICENSE("Dual BSD/GPL");


static struct proc_dir_entry *dprocfs_uc_converter;
static struct proc_dir_entry *dprocfs_text;
static struct proc_dir_entry *dprocfs_statistics;

static char string[PAGE_SIZE] = "";

/* Statistic vars */
static int last_symbol_cnt;
static int total_symbol_cnt;
static int last_conv_symbol_cnt;
static int total_conv_symbol_cnt;


static ssize_t read_text(struct file *file, char __user *pbuf,
			      size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(pbuf, count, ppos,
						string, strlen(string));
}


static ssize_t write_text(struct file *file, const char __user *pbuf,
			       size_t count, loff_t *ppos)
{
	int i = 0;

	last_symbol_cnt = simple_write_to_buffer(string, sizeof(string),
						ppos, pbuf, count);

	if (last_symbol_cnt < 0) {
		pr_info("Wrong string");

		return count;
	}

	string[last_symbol_cnt] = '\0';

	for (i = 0; string[i] != '\0'; i++) {
		if ((string[i] >= 'a') && (string[i] <= 'z')) {
			string[i] += 'A' - 'a';
			last_conv_symbol_cnt++;
		}
	}

	last_symbol_cnt = count - 1;

	total_symbol_cnt += last_symbol_cnt;

	total_conv_symbol_cnt += last_conv_symbol_cnt;

	pr_info("To get converted result print: cat text");

	return count;
}


static ssize_t read_statistics(struct file *file, char __user *pbuf,
			      size_t count, loff_t *ppos)
{
	char tmp[PAGE_SIZE] = "";

	sprintf(tmp, "procfs_uppercase_module statistic:\n"
				"\t%d - symbols entered during last operation\n"
				"\t%d - symbols converted during last operation\n"
				"\t%d - symbols entered since module insertion\n"
				"\t%d - symbols converted since module insertion\n",
				last_symbol_cnt, last_conv_symbol_cnt,
				total_symbol_cnt, total_conv_symbol_cnt);

	return simple_read_from_buffer(pbuf, count, ppos, tmp, strlen(tmp));
}


static const struct file_operations fops_dprocfs_text = {
	.owner = THIS_MODULE,
	.read = read_text,
	.write = write_text
};


static const struct file_operations fops_dprocfs_statistics = {
	.owner = THIS_MODULE,
	.read = read_statistics
};


static int __init procfs_uppercase_module_init(void)
{
	int ret_value = 0;

	dprocfs_uc_converter = proc_mkdir("uc_converter", NULL);
	if (dprocfs_uc_converter == NULL)
		ret_value = -ENOMEM;

	dprocfs_text = proc_create("text", 0644,
				dprocfs_uc_converter, &fops_dprocfs_text);

	dprocfs_statistics = proc_create("statistics", 0444,
				dprocfs_uc_converter, &fops_dprocfs_statistics);

	pr_info("procfs_uppercase_module says: Hello!\n");

	return ret_value;
}



static void __exit procfs_uppercase_module_exit(void)
{
	proc_remove(dprocfs_statistics);
	proc_remove(dprocfs_text);
	proc_remove(dprocfs_uc_converter);

	pr_info("procfs_uppercase_module says: Bye!\n");
}

module_init(procfs_uppercase_module_init);
module_exit(procfs_uppercase_module_exit);
