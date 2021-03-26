// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include "chdev_common.h"

#define BUF_SIZE 32
#define STAT_BUF_SIZE 256
#define MODULE_NAME "sysfs_entry"
#define CLASS_NAME "sys_chdev_entry"
#define STAT_STRING "Total calls - %d, total write chars - %d, total read chars - %d!\n"
#define CLEAN_STRING "clean"

static struct class *attr_class;

static ssize_t buf_clean_store(struct class *class,
		struct class_attribute *attr, const char *pbuf, size_t count)
{
	if (count < strlen(CLEAN_STRING) || memcmp(pbuf, CLEAN_STRING,
				strlen(CLEAN_STRING))) {
		pr_err(MODULE_NAME ": to clean buffer send 'clean' string\n");
		return -1;
	}

	pr_info(MODULE_NAME ": clean buffer\n");
	rd_ptr = wr_ptr;
	buf_len = 0;

	return count;
}

static ssize_t buf_debug_show(struct class *class, struct class_attribute *attr,
		char *st_buf)
{
	return snprintf(st_buf, STAT_BUF_SIZE, STAT_STRING, stat_calls,
			stat_writed, stat_readed);
}

CLASS_ATTR_WO(buf_clean);
CLASS_ATTR_RO(buf_debug);

int sysfs_entry_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (attr_class == NULL) {
		pr_err(MODULE_NAME ": error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_buf_clean);
	if (ret) {
		pr_err(MODULE_NAME ": error creating sysfs class attribute conv\n");
		goto clean_class;
	}

	ret = class_create_file(attr_class, &class_attr_buf_debug);
	if (ret) {
		pr_err(MODULE_NAME ": error creating sysfs class attribute stat\n");
		goto clean_conv;
	}

	pr_info(MODULE_NAME ": init success\n");

	return 0;

clean_conv:
	class_remove_file(attr_class, &class_attr_buf_clean);

clean_class:
	class_destroy(attr_class);
	return ret;
}

void sysfs_entry_exit(void)
{
	class_remove_file(attr_class, &class_attr_buf_debug);
	class_remove_file(attr_class, &class_attr_buf_clean);
	class_destroy(attr_class);

	pr_info(MODULE_NAME ": release!\n");
}
