// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>

#define MAX_STR_LEN    120
#define INPUT_DATA     "input_data"
#define STATISTIC      "stat_data"

struct smbl_count {
unsigned int smbl_count;
unsigned int conv_smbl_count;
unsigned int all_smbl_count;
};


MODULE_AUTHOR("Ihor Stepukh");
MODULE_DESCRIPTION("Uppercase converter using procfs");
MODULE_LICENSE("Dual BSD/GPL");


static struct proc_dir_entry *procfs_conv;
static struct proc_dir_entry *procfs_input;
static struct proc_dir_entry *procfs_stat;

static char str[MAX_STR_LEN] = "";

static struct smbl_count result;


static ssize_t read_data(struct file *file, char __user *pbuf,
						size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(pbuf, count, ppos, str, strlen(str));
}

static ssize_t write_data(struct file *file, const char __user *pbuf,
						size_t count, loff_t *ppos)
{
	int i = 0;

	result.smbl_count = simple_write_to_buffer(str, sizeof(str), ppos,
						pbuf, count);
	if (result.smbl_count < 0) {
		pr_info("Incorrect data");
		return -EIO;
	}

	str[result.smbl_count] = '\0';

	// iterate loop until the end of the string
	while (str[i] != '\0') {
		// if character is in lowercase then subtract 32
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] = str[i] - 32;

		i++;
	}

	result.smbl_count = count - 1;
	result.conv_smbl_count = result.smbl_count;

	result.all_smbl_count += result.smbl_count;

	pr_info("Please look at input_data");

	return count;
}

static ssize_t read_statistics(struct file *file, char __user *pbuf,
				size_t count, loff_t *ppos)
{
	char tmp[MAX_STR_LEN] = "";

	sprintf(tmp, "Statistics:\n"
		"\t%u - Latest entered symbols\n"
		"\t%u - Latest converted symbols\n"
		"\t%u - All entered symbols\n",
		result.smbl_count, result.conv_smbl_count,
		result.all_smbl_count);

	return simple_read_from_buffer(pbuf, count, ppos, tmp, strlen(tmp));
}

static const struct file_operations f_procfs_input = {
	.owner = THIS_MODULE,
	.read = read_data,
	.write = write_data
};

static const struct file_operations f_procfs_stat = {
	.owner = THIS_MODULE,
	.read = read_statistics
};

static int __init module_05_init(void)
{
	int ret = 0;

	procfs_conv = proc_mkdir("conv_to_uppercase", NULL);
	if (procfs_conv == NULL)
		ret = -ENOMEM;

	procfs_input = proc_create(INPUT_DATA, 0644, procfs_conv,
				&f_procfs_input);
	procfs_stat = proc_create(STATISTIC, 0444, procfs_conv, &f_procfs_stat);

	pr_info("UpperCase converter module starts\n");

	return ret;
}

static void __exit module_05_exit(void)
{
	proc_remove(procfs_stat);
	proc_remove(procfs_input);
	proc_remove(procfs_conv);

	pr_info("UpperCase converter module finishes\n");
}

module_init(module_05_init);
module_exit(module_05_exit);


