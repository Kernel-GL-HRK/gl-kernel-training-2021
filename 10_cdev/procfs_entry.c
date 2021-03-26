// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include "chdev_common.h"

#define STAT_BUF_SIZE 256
#define MODULE_NAME "procfs_entry"
#define CONV_DIR_NAME "procfs_chdev_entry"
#define CONV_STAT_NAME "buf_info"
#define STAT_STRING "Total buffer len - %d, chars int buffer - %d, free buffer space - %d!\n"

static struct proc_dir_entry *dir, *ent_stat;
static char buf_stat[STAT_BUF_SIZE];

static int stat_read(struct file *file, char __user *pbuf, size_t count,
		loff_t *ppos)
{
	int ret;

	ret = snprintf(buf_stat, STAT_BUF_SIZE, STAT_STRING, BUF_SIZE,
			buf_len, BUF_SIZE - buf_len);
	return simple_read_from_buffer(pbuf, count, ppos, buf_stat, ret);
}


static const struct file_operations stat_ops = {
	.owner = THIS_MODULE,
	.read = stat_read,
};

int procfs_entry_init(void)
{
	dir = proc_mkdir(CONV_DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err(MODULE_NAME ": error creating directory\n");
		return -ENOMEM;
	}

	ent_stat = proc_create(CONV_STAT_NAME, 0, dir, &stat_ops);

	if (!ent_stat) {
		pr_err(MODULE_NAME ": error creating statistic\n");
		goto clean_dir;
	}

	pr_info(MODULE_NAME ": init success!\n");
	return 0;

clean_dir:
	remove_proc_entry(CONV_DIR_NAME, NULL);
	return -ENOMEM;
}

void procfs_entry_exit(void)
{
	proc_remove(dir);
	pr_info(MODULE_NAME ": release!\n");
}
