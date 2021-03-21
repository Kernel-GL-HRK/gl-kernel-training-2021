// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/mutex.h>


MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("Character device driver for text messaging between users");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

#define PROCFS_INTERNAL_BUF_SIZE 100

static int buffer_size = 1024;
module_param(buffer_size, int, 0);
MODULE_PARM_DESC(buffer_size,
			"Buffer size for messages. Must be bigger than 1024");

static struct proc_dir_entry *chdev_procfs_dir;
static struct proc_dir_entry *chdev_procfs_used_size;
static struct proc_dir_entry *chdev_procfs_buffer_size;

static struct class *class_chdev;
static struct device *device_chdev;
static dev_t devt_chdev;
static struct cdev cdev_obj;

static char *buffer;

static int total_calls_open;
static int total_calls_read;
static int total_calls_write;
static int total_calls_release;
static int total_char_written;

static int buffer_write_cursor;


DEFINE_MUTEX(mutex_buffer_write);


//sysfs
ssize_t show_clean_buff(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	return sprintf(buf, "Chardev buffer info:\n"
				"Buffer size = %i\n"
				"Buffer usage size =  %i\n",
				buffer_size, buffer_write_cursor);
}

ssize_t store_clean_buff(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	buffer_write_cursor = 0;
	buffer[0] = '\0';
	pr_info("Buffer cleaned\n");
	return count;
}


ssize_t show_statistics(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	return sprintf(buf, "Chardev statistics:\n"
				"Total calls open = %i\n"
				"Total calls read =  %i\n"
				"Total calls write = %i\n"
				"Total calls close = %i\n"
				"Total char written = %i\n",
				total_calls_open, total_calls_read,
				total_calls_write, total_calls_release,
				total_char_written);
}



struct device_attribute  device_attr_clean_buff = {
	.attr = { .name = "chdev_clean_buff", .mode = 0644 },
	.show	= show_clean_buff,
	.store	= store_clean_buff
};

struct device_attribute  device_attr_statistics = {
	.attr = { .name = "chdev_statistics", .mode = 0444 },
	.show	= show_statistics,
};

static struct attribute *attrs_chdev[] = {
	&device_attr_clean_buff.attr,
	&device_attr_statistics.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

struct attribute_group device_attr_group_chdev = {
	.attrs = attrs_chdev
};

//sysfs

//procfs
static ssize_t read_chdev_bufsize(struct file *file, char __user *pbuf,
			      size_t count, loff_t *ppos)
{
	char procfs_buf[PROCFS_INTERNAL_BUF_SIZE];

	sprintf(procfs_buf, "Total buffer size = %i\n", buffer_size);
	return simple_read_from_buffer(pbuf, count, ppos,
					procfs_buf, strlen(procfs_buf));
}

static ssize_t read_chdev_used_size(struct file *file, char __user *pbuf,
			      size_t count, loff_t *ppos)
{
	char procfs_buf[PROCFS_INTERNAL_BUF_SIZE];

	sprintf(procfs_buf, "Used buffer size = %i\n", buffer_write_cursor);
	return simple_read_from_buffer(pbuf, count, ppos,
					procfs_buf, strlen(procfs_buf));
}

static const struct file_operations fops_chdev_bufsize = {
	.owner = THIS_MODULE,
	.read = read_chdev_bufsize
};

static const struct file_operations fops_chdev_used_size = {
	.owner = THIS_MODULE,
	.read = read_chdev_used_size
};

//procfs


static int open_chdev(struct inode *inode, struct file *file)
{
	total_calls_open++;
	return 0;
}

static int close_chdev(struct inode *inode, struct file *file)
{
	total_calls_release++;
	return 0;
}

static ssize_t read_chdev(struct file *file, char __user *pbuf,
			      size_t count, loff_t *ppos)
{
	total_calls_read++;
	return simple_read_from_buffer(pbuf, count, ppos,
		buffer, buffer_size - (buffer_size - buffer_write_cursor));
}

static ssize_t write_chdev(struct file *file, const char __user *pbuf,
			       size_t count, loff_t *ppos)
{
	ssize_t msg_size = 0;

	total_calls_write++;
	if (count > buffer_size) {
		pr_err("Too large msg\n");
		return -EINVAL;
	}

	if (buffer_write_cursor + count > buffer_size)
		buffer_write_cursor = 0;


	mutex_lock(&mutex_buffer_write);
	msg_size = simple_write_to_buffer(buffer + buffer_write_cursor,
			buffer_size - buffer_write_cursor, ppos, pbuf, count);
	mutex_unlock(&mutex_buffer_write);

	if (msg_size < 0) {
		pr_err("Can't copy your string\n");
		return msg_size;
	}

	buffer_write_cursor += msg_size;
	total_char_written += msg_size - 1;

	return msg_size;
}


static const struct file_operations fops_chdev = {
	.owner = THIS_MODULE,
	.open = open_chdev,
	.read = read_chdev,
	.write = write_chdev,
	.release = close_chdev,
};


static int __init chdev_init(void)
{
	int must_check = 0;

	if (buffer_size < 1024)
		return -EINVAL;

	pr_info("buffer size %i\n", buffer_size);
	buffer = kmalloc_array(buffer_size, sizeof(buffer), GFP_KERNEL);
	if (buffer == NULL) {
		pr_err("error while allocating buffer\n");
		return -ENOMEM;
	}

	//procfs creation
	chdev_procfs_dir = proc_mkdir("chdev_dir", NULL);
	if (chdev_procfs_dir == NULL) {
		pr_err("error creating procfs dir\n");
		goto free_buffer;
	}

	chdev_procfs_buffer_size = proc_create("chdev_buffer_size", 0444,
					chdev_procfs_dir, &fops_chdev_bufsize);
	if (chdev_procfs_buffer_size == NULL) {
		pr_err("error creating procfs file buffer size\n");
		goto procfs_remove_dir;
	}

	chdev_procfs_used_size = proc_create("chdev_used_size", 0444,
				chdev_procfs_dir, &fops_chdev_used_size);
	if (chdev_procfs_used_size == NULL) {
		pr_err("error creating procfs file used size\n");
		goto procfs_remove_buffer_size;
	}


	must_check = alloc_chrdev_region(&devt_chdev, 0, 1, "chdev_region");
	if (must_check) {
		pr_err("Error while add char device\n");
		goto procfs_remove;
	}

	//class and device creation
	class_chdev = class_create(THIS_MODULE, "chdev_dir");
	if (class_chdev == NULL) {
		pr_err("Error while creating a class\n");
		goto unregister_char_dev_region;
	}

	device_chdev = device_create(class_chdev, NULL, devt_chdev, NULL,
							"chdev_device");
	if (device_chdev == NULL) {
		pr_err("Error while creating a device\n");
		goto class_remove;
	}


	must_check = sysfs_create_group(&(device_chdev->kobj),
					&device_attr_group_chdev);
	if (must_check) {
		pr_err("Error while creating a group of files\n");
		goto device_remove;
	}

	cdev_init(&cdev_obj, &fops_chdev);
	must_check = cdev_add(&cdev_obj, devt_chdev, 1);
	if (must_check) {
		pr_err("Error while add char device\n");
		goto sysfs_group_remove;
	}


	pr_info("module loaded\n");
	return 0;


sysfs_group_remove:
	sysfs_remove_group(&(device_chdev->kobj),
						&device_attr_group_chdev);
device_remove:
	device_destroy(class_chdev, devt_chdev);
class_remove:
	class_destroy(class_chdev);
unregister_char_dev_region:
	unregister_chrdev_region(devt_chdev, 1);
procfs_remove:
	proc_remove(chdev_procfs_used_size);
procfs_remove_buffer_size:
	proc_remove(chdev_procfs_buffer_size);
procfs_remove_dir:
	proc_remove(chdev_procfs_dir);
free_buffer:
	kfree(buffer);
	return -ENOMEM;
}

static void __exit chdev_exit(void)
{
	unregister_chrdev_region(devt_chdev, 1);
	cdev_del(&cdev_obj);
	sysfs_remove_group(&(device_chdev->kobj),
						&device_attr_group_chdev);
	device_destroy(class_chdev, devt_chdev);
	class_destroy(class_chdev);
	proc_remove(chdev_procfs_used_size);
	proc_remove(chdev_procfs_buffer_size);
	proc_remove(chdev_procfs_dir);
	kfree(buffer);
	pr_info("module exited\n");
}

module_init(chdev_init);
module_exit(chdev_exit);
