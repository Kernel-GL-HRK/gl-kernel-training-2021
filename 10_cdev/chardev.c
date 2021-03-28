// SPDX-License-Identifier: MIT and GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define  DEVICE_NAME "char_dev"
#define  CLASS_NAME  "char_class"
#define  LENGTH_DEF	1024
#define	 PROC_SYS_BUF	16
#define  PROC_DIRECTORY  "proc_chdev"
#define  PROC_FILENAME_1  "buf_volume"
#define  PROC_FILENAME_2  "buf_size"
#define  SYS_FILE_NAME	  "sys_char"
#define CLASS_ATTR(_name, _mode, _show, _store)	\
struct class_attribute class_attr_##_name =	\
__ATTR(_name, _mode, _show, _store)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Kotsiubailo");
MODULE_DESCRIPTION("Task10 chardev with sysfs and procfs");
MODULE_VERSION("0.1");

static int    majorNumber;
static char *message;
static char *size_buffer;
static char *volume_buffer;
static short  size_of_message;
static int numberOpens;
static struct class *charClass;
static struct device *charDevice;
static unsigned int length = LENGTH_DEF;
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_file2;
module_param(length, int, 0);

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int volume_read(struct file *file_p, char __user *buffer, size_t length,
		     loff_t *offset);
static int size_read(struct file *file_p, char __user *buffer, size_t length,
		     loff_t *offset);

static struct file_operations volume_fops = {
	.read = volume_read,
};

static struct file_operations size_fops = {
	.read = size_read,
};

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static ssize_t sys_show(struct class *class, struct class_attribute *attr,
			char *buf)
{
	int len;

	len = sprintf(buf, "%d", numberOpens);
	if (len == 0) {
		pr_err("Failed to convert int to string\n");
		return 0;
	}
	pr_info("%s", buf);
	pr_info("read %ld\n", (long)strlen(buf));
	return strlen(buf);
}

static ssize_t clear_buf(struct class *class, struct class_attribute *attr,
			const char *buf, size_t count)
{
	size_of_message = 0;
	return count;
}

static int create_buffer(void)
{
	size_buffer = kmalloc(PROC_SYS_BUF, GFP_KERNEL);
	if (size_buffer == NULL)
		return -ENOMEM;

	volume_buffer = kmalloc(PROC_SYS_BUF, GFP_KERNEL);
	if (volume_buffer == NULL)
		return -ENOMEM;

	return 0;
}

static void cleanup_buffer(char *buff)
{
	kfree(buff);
	buff = NULL;
}

static int create_proc_dir(void)
{
	proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
	if (proc_dir == NULL)
		return -EFAULT;

	proc_file = proc_create(PROC_FILENAME_1, S_IFREG | 0444, proc_dir,
				&volume_fops);
	if (proc_file == NULL)
		return -EFAULT;

	proc_file2 = proc_create(PROC_FILENAME_2, S_IFREG | 0444, proc_dir,
				&size_fops);
	if (proc_file == NULL)
		return -EFAULT;

	return 0;
}
static int size_read(struct file *file_p, char __user *buffer, size_t _length,
		     loff_t *offset)
{
	size_t left;
	int len;

	if (*offset > 0)
		return 0;

	len = sprintf(size_buffer, "%d", size_of_message);
	if (len == 0) {
		pr_err("Failed to convert int to string\n");
		return 0;
	}

	left = copy_to_user(buffer, size_buffer, len);
	if (left)
		pr_err("Failed to read %u from %u chars\n", left,
			length);
	else
		pr_notice("Read %u chars, %s\n", len, buffer);

	return len;
}

static int volume_read(struct file *file_p, char __user *buffer, size_t _length,
		     loff_t *offset)
{
	size_t left;
	int len;

	if (*offset > 0)
		return 0;

	len = sprintf(volume_buffer, "%d", length);
	if (len == 0) {
		pr_err("Failed to convert int to string\n");
		return 0;
	}

	left = copy_to_user(buffer, volume_buffer, len);
	if (left)
		pr_err("Failed to read %u from %u chars\n", left,
			length);
	else
		pr_notice("Read %u chars, %s\n", len, buffer);

	return len;
}

static void cleanup_proc(void)
{
	if (proc_file2) {
		remove_proc_entry(PROC_FILENAME_2, proc_dir);
		proc_file = NULL;
	}

	if (proc_file) {
		remove_proc_entry(PROC_FILENAME_1, proc_dir);
		proc_file = NULL;
	}

	if (proc_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}
}

CLASS_ATTR(numop, 0444, &sys_show, NULL);
CLASS_ATTR(clear, 0664, NULL, &clear_buf);

static struct class *sys_class;

int create_sys_file(void)
{
	int res;

	sys_class = class_create(THIS_MODULE, SYS_FILE_NAME);
	if (IS_ERR(sys_class)) {
		pr_err("bad class create\n");
		return -EINVAL;
	}

	res = class_create_file(sys_class, &class_attr_numop);
	if (res != 0) {
		pr_err("bad sys_char class file create\n");
		class_destroy(sys_class);
		return -EINVAL;
	}

	res = class_create_file(sys_class, &class_attr_clear);
	if (res != 0) {
		pr_err("bad file 'total_characters_processed' create\n");
		class_destroy(sys_class);
		return -EINVAL;
	}

	pr_info("sys_char class in sysfs initialized\n");
	return res;
}

void clean_sys_file(void)
{
	class_remove_file(sys_class, &class_attr_clear);
	class_remove_file(sys_class, &class_attr_numop);
	class_destroy(sys_class);
}

static int __init char_init(void)
{
	int err;
	if (length > LENGTH_DEF) {
		pr_err("Wrong length of buffer");
		return -EINVAL;
	}

	pr_info("Initializing task10 module\n");

	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber < 0) {
		pr_err("Failed to register a major number\n");
		return majorNumber;
	}

	pr_info("Registered correctly with major number %d\n", majorNumber);

	charClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(charClass)) {
		unregister_chrdev(majorNumber, DEVICE_NAME);
		pr_err("Failed to register device class\n");
		return PTR_ERR(charClass);
	}

	pr_info("Device class registered correctly\n");

	charDevice = device_create(charClass, NULL,
				MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

	if (IS_ERR(charDevice)) {
		class_destroy(charClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		pr_err("Failed to create the device\n");
		return PTR_ERR(charDevice);
	}

	message = kmalloc(length * sizeof(char), GFP_KERNEL);
	if (message == NULL)
		return -ENOMEM;

	pr_info("Buffer size is %d\n", length);
	pr_info(" Char device class created correctly\n");

	err = create_buffer();
	if (err)
		goto error;

	err = create_proc_dir();
	if (err)
		goto error;
	pr_notice("Procfs volume and size loaded\n");

	err = create_sys_file();
	if (err)
		goto error;

	return 0;

error:
	pr_err("Failed to load\n");
	cleanup_proc();
	cleanup_buffer(volume_buffer);
	cleanup_buffer(size_buffer);
	return err;
}

static void __exit char_exit(void)
{
	device_destroy(charClass, MKDEV(majorNumber, 0));
	class_unregister(charClass);
	class_destroy(charClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	kfree(message);
	cleanup_proc();
	cleanup_buffer(volume_buffer);
	cleanup_buffer(size_buffer);
	clean_sys_file();
	pr_info("Bye!\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	numberOpens++;
	pr_info("Device has been opened %d times\n", numberOpens);
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
			loff_t *offset)
{
	int ret = 0;

	if (*offset)
		return 0;

	ret = copy_to_user(buffer, message, size_of_message);
	if (ret != 0)
		return -EFAULT;


	*offset = size_of_message;

	return *offset;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
			loff_t *offset)
{
	int ret;

	if (len > sizeof(message) - 1)
		size_of_message = sizeof(message) - 1;

	ret = copy_from_user(message, buffer, len);
	if (ret != 0)
		return -EFAULT;

	message[len] = '\0';
	size_of_message = len;

	pr_info("Received %zu characters from the user\n", len);
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	pr_info("Device successfully closed\n");
	return 0;
}

module_init(char_init);
module_exit(char_exit);
