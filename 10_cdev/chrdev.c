// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include "chdev_common.h"
#include "procfs_entry.h"
#include "sysfs_entry.h"

#define CLASS_NAME "chrdev"
#define DEVICE_NAME "chrdev_example"
#define STAT_BUF_SIZE 256
#define MODULE_NAME "chdev_mod"

static struct class *pclass;
static struct device *pdev;

static int major;
static int is_open;

static char buf[BUF_SIZE];
int stat_calls, stat_readed, stat_writed, buf_len, wr_ptr, rd_ptr;

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("chrdev: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("chrdev: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("chrdev: device closed\n");
	return 0;
}

static size_t get_part_size(size_t size, int ptr)
{
	size_t part_size;

	/* Get lenght no longer then the endge of the buffer */
	if (size > BUF_SIZE - ptr)
		part_size = BUF_SIZE - ptr;
	else
		part_size = size;

	return part_size;
}

static ssize_t dev_read(struct file *filep, char *pbuf, size_t count,
		loff_t *offset)
{
	int ret;
	size_t part_size, size = 0;

	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("chrdev: read from device %d:%d\n", imajor(filep->f_inode),
			iminor(filep->f_inode));

	stat_calls++;

	if (count > buf_len)
		count = buf_len;

	while (size != count) {
		part_size = get_part_size(count - size, rd_ptr);

		ret = copy_to_user(pbuf + size, buf + rd_ptr, part_size);

		if (!ret) {
			rd_ptr += part_size;
			size += part_size;
			buf_len -= part_size;
		}

		/* Check the and of the buffer for cicling */
		if (rd_ptr == BUF_SIZE)
			rd_ptr = 0;
	}

	stat_readed += count;

	return count;
}

static ssize_t dev_write(struct file *filep, const char *pbuf, size_t count,
		loff_t *offset)
{
	int ret;
	size_t part_size, size = 0;

	pr_info("chrdev: write to file %s\n", filep->f_path.dentry->d_iname);
	pr_info("chrdev: write to device %d:%d\n", imajor(filep->f_inode),
			iminor(filep->f_inode));

	stat_calls++;

	if (buf_len == BUF_SIZE) {
		pr_err(MODULE_NAME ": buffer is full\n");
		return -1;
	}

	if (count > BUF_SIZE - buf_len)
		count = BUF_SIZE - buf_len;

	while (size != count) {
		part_size = get_part_size(count - size, wr_ptr);

		ret = copy_from_user(buf + wr_ptr, pbuf + size, part_size);

		if (!ret) {
			wr_ptr += part_size;
			size += part_size;
			buf_len += part_size;
		}

		/* Check the and of the buffer for cicling */
		if (wr_ptr == BUF_SIZE)
			wr_ptr = 0;
	}

	stat_writed += count;

	return count;
}

static const struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int __init chrdev_init(void)
{
	int ret;

	is_open = 0;

	ret = procfs_entry_init();

	if (ret)
		return ret;

	ret = sysfs_entry_init();

	if (ret)
		goto err_sysfs;

	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_err("register_chrdev failed: %d\n", major);
		ret = major;
		goto err_reg_chrdev;
	}
	pr_info("chrdev: register_chrdev ok, major = %d\n", major);

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		pr_err("chrdev: class_create failed\n");
		ret = PTR_ERR(pclass);
		goto err_class;
	}
	pr_info("chrdev: device class created successfully\n");

	pdev = device_create(pclass, NULL, MKDEV(major, 0), NULL, CLASS_NAME
			"0");
	if (IS_ERR(pdev)) {
		pr_err("chrdev: device_create failed\n");
		ret = PTR_ERR(pdev);
		goto err_dev;
	}
	pr_info("chrdev: device node created successfully\n");

	pr_info("chrdev: module loaded\n");

	return 0;

err_dev:
	class_destroy(pclass);
err_class:
	unregister_chrdev(major, DEVICE_NAME);
err_sysfs:
	procfs_entry_exit();
err_reg_chrdev:
	sysfs_entry_exit();

	return ret;
}

static void __exit chrdev_exit(void)
{
	pr_info("start release module...\n");
	device_destroy(pclass, MKDEV(major, 0));
	class_destroy(pclass);
	unregister_chrdev(major, DEVICE_NAME);

	sysfs_entry_exit();
	procfs_entry_exit();

	pr_info("chrdev: module exited\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");
