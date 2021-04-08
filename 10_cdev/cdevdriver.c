
/*	SPDX-License-Identifier: GPL-3.0	*/
/**/

#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/kfifo.h>

#define CLASS_NAME	"chat_dev"
#define DEVICE_NAME	"users_chat_dev"
#define BUFFER_SIZE	1024
#define PROCFS_NODE "buffer_statistics"

/* lock for read access */
static DEFINE_MUTEX(read_lock);
/* lock for write access */
static DEFINE_MUTEX(write_lock);

static struct class *pclass;
static struct device *pdev;

static int major;
static int is_open;

static int data_size;
static int buffer_size = BUFFER_SIZE;

struct kfifo_rec_ptr_2 test;

module_param(buffer_size, int, 0);

static ssize_t buf_stat_read(struct file *file, char __user *pbuf,
	size_t count, loff_t *ppos)
{
	char *buf;
	size_t len = 0;
	ssize_t ret;

	if (*ppos != 0)
		return 0;
	if (count < sizeof(buf))
		return -ENOSPC;
	buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	len += snprintf(buf + len, PAGE_SIZE - len,
		"BUFFER SIZE: \t\t %d\n", buffer_size);
	len += snprintf(buf + len, PAGE_SIZE - len,
		"BUFFER USED SIZE: \t %d\n", data_size);
	ret = simple_read_from_buffer(pbuf, len, ppos, buf, PAGE_SIZE);
	kfree(buf);
	return ret;
}

static const struct file_operations stat_ops = {
	.owner = THIS_MODULE,
	.read = buf_stat_read,
};

static struct proc_dir_entry *dir;
static struct proc_dir_entry *proc_stat;

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer,
	size_t len, loff_t *offset)
{
	int ret;
	unsigned int copied;

	pr_info("read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("read from device %d:%d\n", imajor(filep->f_inode),
		iminor(filep->f_inode));

	if (mutex_lock_interruptible(&read_lock))
		return -ERESTARTSYS;
	ret = kfifo_to_user(&test, buffer, len, &copied);
	data_size -= copied;
	pr_info("read from buffer %d\n", copied);
	pr_info("used buffer remains %d\n", data_size);
	mutex_unlock(&read_lock);
	return ret ? ret : copied;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
	size_t len, loff_t *offset)
{
	int ret;
	unsigned int copied;

	pr_info("write to file %s\n", filep->f_path.dentry->d_iname);
	pr_info("write to device %d:%d\n", imajor(filep->f_inode),
		iminor(filep->f_inode));

	if (kfifo_avail(&test) < len) {
		pr_info("queue full, skipped\n");
		return -ENOMEM;
	}

	if (mutex_lock_interruptible(&write_lock))
		return -ERESTARTSYS;
	ret = kfifo_from_user(&test, buffer, len, &copied);
	data_size += copied;
	pr_info("write to buffer %d\n", copied);
	pr_info("used buffer %d\n", data_size);
	mutex_unlock(&write_lock);
	return ret ? ret : copied;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
	.llseek = noop_llseek,
};

static int chrdev_init(void)
{
	int ret;

	is_open = 0;
	data_size = 0;

	buffer_size = (buffer_size < BUFFER_SIZE) ?
		BUFFER_SIZE : buffer_size;
	pr_info("buffer size = %d\n", buffer_size);

	ret = kfifo_alloc(&test, buffer_size, GFP_KERNEL);
	if (ret) {
		pr_err("error kfifo alloc\n");
		return ret;
	}

	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_err("register_chrdev failed: %d\n", major);
		ret = major;
		goto init_exit0;
	}
	pr_info("register_chrdev ok, major = %d\n", major);

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		pr_err("class_create failed\n");
		ret = PTR_ERR(pclass);
		goto init_exit1;
	}
	pr_info("device class created successfully\n");

	pdev = device_create(pclass, NULL,
		MKDEV(major, 0), NULL, CLASS_NAME"0");
	if (IS_ERR(pdev)) {
		pr_err("device_create failed\n");
		ret = PTR_ERR(pdev);
		goto init_exit2;
	}
	pr_info("device node created successfully\n");

	dir = proc_mkdir(DEVICE_NAME, NULL);
	if (dir == NULL) {
		pr_err("error creating procfs directory\n");
		ret = -ENOMEM;
		goto init_exit3;
	}

	proc_stat = proc_create(PROCFS_NODE, 0444, dir, &stat_ops);
	if (proc_stat == NULL) {
		pr_err("error creating procfs stat\n");
		ret = -ENOMEM;
		goto init_exit4;
	}

	pr_info("module loaded\n");
	return 0;

init_exit4:
	remove_proc_entry(DEVICE_NAME, NULL);
init_exit3:
	device_destroy(pclass, MKDEV(major, 0));
init_exit2:
	class_destroy(pclass);
init_exit1:
	unregister_chrdev(major, DEVICE_NAME);
init_exit0:
	kfifo_free(&test);
	return ret;
}

static void chrdev_exit(void)
{
	proc_remove(dir);
	device_destroy(pclass, MKDEV(major, 0));
	class_destroy(pclass);
	unregister_chrdev(major, DEVICE_NAME);
	kfifo_free(&test);
	pr_info("module exited\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Character device example driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
