// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 10_cdev task
 *
 * Character device driver for text messaging between users
 * Driver has 1kB buffer default size, it can be increased by setting module
 * parameter buffer_size.
 * Module has sysfs and procfs interfaces for buffer cleaning and printing some
 * debug information.
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>

#include "gl_msg_buf.h"
#include "gl_procfs.h"
#include "gl_sysfs.h"

#define CLASS_NAME "txtmsg"
#define DEVICE_NAME "txtmsg_dev"

#define MIN_BUFFER_SIZE 32
#define DEF_BUFFER_SIZE 1024

static struct class *class;
static struct device *dev;

static int major;
static bool dev_opened;
struct msg_buffer msg_buffer;

static size_t bytes_read;
static size_t bytes_written;

static uint buffer_size = DEF_BUFFER_SIZE;
module_param(buffer_size, uint, 0);

int gl_clean_buffer(void)
{
	return gl_msg_buffer_clean();
}

static int dev_open(struct inode *inode, struct file *file)
{
	if (dev_opened == true)
		return -EBUSY;

	dev_opened = true;
	return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
	dev_opened = false;
	return 0;
}

static ssize_t dev_read(struct file *file, char *buf, size_t len,
	loff_t *offset)
{
	if (*offset > 0)
		return 0;

	*offset = gl_msg_buffer_read(buf, len);
	bytes_read += *offset;

	return *offset;
}

static ssize_t dev_write(struct file *file, const char *buf, size_t len,
	loff_t *offset)
{
	len = gl_msg_buffer_write(buf, len);
	bytes_written += len;

	return len;
}

static const struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write
};

static int gl_cdev_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_err("GL chrdev register failed!\n");
		return major;
	}

	class = class_create(THIS_MODULE, CLASS_NAME);
	if (class == NULL) {
		pr_err("GL chrdev class create failed!\n");
		goto error_class_create;
	}

	dev = device_create(class, NULL, MKDEV(major, 0), NULL, CLASS_NAME"0");
	if (dev == NULL) {
		pr_err("GL chrdev device create failed!\n");
		goto error_dev_create;
	}

	return 0;

error_dev_create:
	class_destroy(class);
error_class_create:
	unregister_chrdev(major, DEVICE_NAME);
	return -1;
}

static void gl_cdev_deinit(void)
{
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, DEVICE_NAME);
}

static int __init gl_cdev_module_init(void)
{
	int ret;

	pr_info("GL Kernel Training 2021\n");

	ret = gl_msg_buffer_init(buffer_size);
	if (ret != 0)
		return ret;

	ret = gl_cdev_init();
	if (ret != 0) {
		pr_err("GL Unable to create chrdev!\n");
		goto no_cdev;
	}

	ret = gl_procfs_init(buffer_size, gl_get_msg_buffer_len());
	if (ret != 0) {
		pr_err("GL Unable to create procfs!\n");
		goto no_procfs;
	}

	ret = gl_sysfs_init(&bytes_read, &bytes_written);
	if (ret != 0) {
		pr_err("GL Unable to create sysfs!\n");
		goto no_sysfs;
	}

	return ret;

no_sysfs:
	gl_procfs_deinit();
no_procfs:
	gl_cdev_deinit();
no_cdev:
	gl_msg_buffer_free();
	return ret;
}

static void __exit gl_cdev_module_exit(void)
{
	pr_info("Exit from 10_cdev module\n");
	gl_sysfs_deinit();
	gl_procfs_deinit();
	gl_cdev_deinit();
	gl_msg_buffer_free();
}

module_init(gl_cdev_module_init);
module_exit(gl_cdev_module_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 10_cdev");
MODULE_LICENSE("GPL");
