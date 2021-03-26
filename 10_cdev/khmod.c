// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/cdev.h>
#include <linux/fs.h>

MODULE_DESCRIPTION("Chardev message buffer");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static unsigned int buffer_size = 1024;
module_param(buffer_size, uint, 0);
MODULE_PARM_DESC(buffer_size, "Messages buffer size");

struct stats {
	unsigned int open_cnt;
	unsigned int release_cnt;
	unsigned int read_cnt;
	unsigned int write_cnt;
};

static struct class *attr_class;
static struct device *device_chdev;

static struct cdev my_cdev;
struct stats cdev_stats;
dev_t my_dev;

char *buffer;

static ssize_t clean_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	snprintf(buf, PAGE_SIZE,
		"Opened - %d, Released - %d, Read - %d, Write - %d\n",
		cdev_stats.open_cnt, cdev_stats.release_cnt,
		cdev_stats.read_cnt, cdev_stats.write_cnt);

	memset(buffer, 0, buffer_size);

	return strlen(buf);
}

static ssize_t buff_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	snprintf(buf, PAGE_SIZE, "Used buffer volume - %d, Buffersize - %d\n",
				strlen(buffer), buffer_size);

	return strlen(buf);
}

struct class_attribute lass_attr_clean_show = {
	.attr = { .name = "clean_show", .mode = 0444 },
	.show	= &clean_show,
};

struct class_attribute class_attr_buff_show = {
	.attr = { .name = "buff_show", .mode = 0444 },
	.show	= &buff_show,
};

static int cdev_open(struct inode *inode, struct file *file)
{
	cdev_stats.open_cnt++;
	return 0;
}

static int cdev_release(struct inode *inode, struct file *file)
{
	cdev_stats.release_cnt++;
	return 0;
}

static ssize_t cdev_read(struct file *file, char __user *buf,
			 size_t count, loff_t *loff)
{
	cdev_stats.read_cnt++;
	return simple_read_from_buffer(buf, count, loff,
		buffer, strlen(buffer));
}

static ssize_t cdev_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *loff)
{

	char *tmp;
	int size;

	cdev_stats.write_cnt++;

	if (count > buffer_size) {
		pr_err("Message to large\n");
		return -EINVAL;
	}

	tmp = kzalloc(sizeof(*tmp) * count, GFP_KERNEL);
	if (tmp == NULL)
		return -ENOMEM;

	size = simple_write_to_buffer(tmp, count, loff, buf, count);
	if (!size)
		return -EFAULT;

	if (strlen(buffer) + count > buffer_size) {
		strncpy(buffer, tmp, buffer_size);
		strcat(buffer, "\n");
		pr_info("Buffer crowded, old data overwritten\n");
	} else {
		strcat(buffer, tmp);
		strcat(buffer, "\n");
	}

	kfree(tmp);
	return size;
}

static const struct file_operations pass_gen_fops = {
	.open		=	&cdev_open,
	.release	=	&cdev_release,
	.read		=	&cdev_read,
	.write		=	&cdev_write,
	.owner		=	THIS_MODULE,
};

static int __init chardev_init(void)
{
	int ret = 0;

	buffer = kzalloc(sizeof(*buffer) * (buffer_size + 1), GFP_KERNEL);
	if (buffer == NULL)
		return -ENOMEM;

	attr_class = class_create(THIS_MODULE, "khdir");

	if (attr_class == NULL) {
		ret = -ENOMEM;
		pr_err("%s: error creating sysfs class\n", THIS_MODULE->name);
		goto free_buff;
	}

	ret = class_create_file(attr_class, &lass_attr_clean_show);
	if (ret) {
		pr_err("%s: error creating sysfs class rw attribute\n",
			THIS_MODULE->name);
		goto create_attr_clean_err;
	}

	ret = class_create_file(attr_class, &class_attr_buff_show);
	if (ret) {
		pr_err("%s: error creating sysfs class r attribute\n",
			THIS_MODULE->name);
		goto create_attr_buff_err;
	}

	ret = alloc_chrdev_region(&my_dev, 0, 1, "chdev_region");
	if (ret) {
		pr_err("%s: alloc_chrdev_region error", THIS_MODULE->name);
		goto chardev_region_err;
	}

	device_chdev = device_create(attr_class, NULL, my_dev, NULL, "khmod");

	if (IS_ERR(device_chdev)) {
		pr_err("%s: device_create error", THIS_MODULE->name);
		goto chardev_dev_create_err;
	}

	cdev_init(&my_cdev, &pass_gen_fops);
	ret = cdev_add(&my_cdev, my_dev, 1);
	if (ret) {
		pr_err("%s: cdev_init error", THIS_MODULE->name);
		goto chardev_dev_create_err;
	}

	pr_info("Registered device with %d:%d\n", MAJOR(my_dev), MINOR(my_dev));
	pr_info("%s: loaded!\n", THIS_MODULE->name);

	return 0;

chardev_dev_create_err:
	device_destroy(attr_class, my_dev);
chardev_region_err:
	class_remove_file(attr_class, &class_attr_buff_show);
create_attr_buff_err:
	class_remove_file(attr_class, &lass_attr_clean_show);
create_attr_clean_err:
	class_destroy(attr_class);
free_buff:
	kfree(buffer);

	return ret;
}

static void __exit chardev_exit(void)
{
	cdev_del(&my_cdev);
	device_destroy(attr_class, my_dev);
	unregister_chrdev_region(my_dev, 1);
	class_remove_file(attr_class, &class_attr_buff_show);
	class_remove_file(attr_class, &lass_attr_clean_show);
	class_destroy(attr_class);

	kfree(buffer);
	pr_info("%s: removed!\n", THIS_MODULE->name);
}

module_init(chardev_init);
module_exit(chardev_exit);

