// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h> // required by all modules
#include <linux/kernel.h> // required for sysinfo
#include <linux/init.h> // used by module_init, module_exit macros
#include <linux/jiffies.h> // where jiffies and its helpers reside
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/debugfs.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/spinlock.h>

MODULE_DESCRIPTION("Basic module demo: char. dev");
MODULE_AUTHOR("AlexShlikhta oleksandr.shlikhta@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

static uint32_t buffer_size = 1024; // bytes
module_param(buffer_size, int, 0444);
MODULE_PARM_DESC(buffer_size, "size of messages buffer in bytes");

#define CLASS_ATTR(_x) &(class_attr_##_x)

#define CLASS_ATTR_CREATE(_name, _mode, _show, _store)                     \
	struct class_attribute class_attr_##_name = __ATTR(_name, _mode, _show, _store)

DEFINE_SPINLOCK(lock);

struct alloc_t {
	unsigned reg_cdev : 1;
	unsigned cl_create : 1;
	unsigned dev_create : 1;
	unsigned buff_alloc : 1;
	unsigned used_buf_vol_attr_create : 1;
	unsigned buffer_size_attr_create : 1;
	unsigned buf_clean_attr_create : 1;
	unsigned journal_attr_create : 1;
} alloc;

static LIST_HEAD(journal_list);

struct _journal_head_t {
	struct list_head head;
	char *note;
	uint64_t note_jiffies;
};

static char *buffer;
static uint8_t files_counter;
uint32_t major_num;
struct class *cl;
static struct device *pdev;
static int used_buf_vol;

#define DEVICE_NAME "cdev"
#define CLASS_NAME "messenger"

#define FIRST_MINOR_NUM 0
#define NUM_OF_MINOR_NUM 1

int add_note_to_list(char *text)
{
	struct _journal_head_t *tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);

	if (tmp == NULL) {
		pr_err("Can't add a note to journal:(\n");
		return -ENOMEM;
	}

	tmp->note = kstrdup(text, GFP_KERNEL);
	tmp->note_jiffies = jiffies;

	list_add_tail(&(tmp->head), &journal_list);

	return 0;
}

/**
 * cdev_open() - callback for open() file operation
 * @inode: information to manipulate the file (unused)
 * @file: VFS file opened by a process
 *
 *
 */
static int cdev_open(struct inode *inode, struct file *file)
{
	files_counter++;

	return add_note_to_list("File was opened");
}

/**
 * cdev_release() - file close() callback
 * @inode: information to manipulate the file (unused)
 * @file: VFS file opened by a process
 */
static int cdev_release(struct inode *inode, struct file *file)
{
	files_counter--;

	return add_note_to_list("File was closed");
}

/**
 * cdev_read() - called on file read() operation
 * @file: VFS file opened by a process
 * @buf:
 * @count:
 * @loff:
 */
static ssize_t cdev_read(struct file *file, char __user *buf, size_t count,
			 loff_t *loff)
{
	int status = add_note_to_list("File was read");

	if (IS_ERR_VALUE(status))
		return status;

	if (used_buf_vol == 0)
		pr_info("Buffer is empty:(\n");

	return simple_read_from_buffer(buf, count, loff, buffer,
				       strlen(buffer));
}

/**
 * cdev_write() - callback for file write() operation
 * @file: VFS file opened by a process
 * @buf:
 * @count:
 * @loff:
 */
static ssize_t cdev_write(struct file *file, const char __user *buf,
			  size_t count, loff_t *loff)
{
	uint32_t n = 0;
	int status = 0;
	char *tmp_note;
	char *tmp = kzalloc(sizeof(*tmp) * count, GFP_KERNEL);

	if (tmp == NULL) {
		pr_err("Can't write to buffer:(\n");
		return 0;
	}

	n = simple_write_to_buffer(tmp, count, loff, buf, count);

	spin_lock(&lock);

	if (count > buffer_size) {
		status = add_note_to_list("**Input string too large");
		if (IS_ERR_VALUE(status))
			return status;
	}

	if ((strlen(buffer) + count) >= buffer_size) {
		strncpy(buffer, tmp, buffer_size);
		status = add_note_to_list("**Buffer was overrided");
		if (IS_ERR_VALUE(status))
			return status;

	} else {
		strcat(buffer, "\n");
		strcat(buffer, tmp);
	}

	used_buf_vol = strlen(buffer);

	spin_unlock(&lock);

	tmp_note = kzalloc(sizeof(*tmp) * (strlen(tmp) +
					   strlen("'' was written to buffer")),
			   GFP_KERNEL);
	if (tmp_note == NULL) {
		pr_err("Can't write to journal:(\n");
		return 0;
	}

	sprintf(tmp_note, "'%s' was written to buffer",
		count > buffer_size ? buffer : tmp);

	status = add_note_to_list(tmp_note);
	if (IS_ERR_VALUE(status))
		return status;

	kfree(tmp);
	kfree(tmp_note);

	return n;
}

// This structure is partially initialized here
// and the rest is initialized by the kernel after call
// to cdev_init()
static const struct file_operations cdev_fops = {
	.open = &cdev_open,
	.release = &cdev_release,
	.read = &cdev_read,
	.write = &cdev_write,
	// required to prevent module unloading while fops are in use
	.owner = THIS_MODULE,
};

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t used_buf_vol_show(struct class *class,
				 struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", used_buf_vol);

	return strlen(buf);
}
CLASS_ATTR_CREATE(used_buf_vol, 0444, &used_buf_vol_show, NULL);

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t buffer_size_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", buffer_size);

	return strlen(buf);
}
CLASS_ATTR_CREATE(buffer_size, 0444, &buffer_size_show, NULL);

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t buf_clean_show(struct class *class, struct class_attribute *attr,
			      char *buf)
{
	int status = add_note_to_list("Buffer was cleared");

	if (IS_ERR_VALUE(status))
		return status;

	memset(buffer, 0, buffer_size);
	used_buf_vol = strlen(buffer);
	sprintf(buf, "Buffer was cleared\n");

	return strlen(buf);
}
CLASS_ATTR_CREATE(buf_clean, 0444, &buf_clean_show, NULL);

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t journal_show(struct class *class, struct class_attribute *attr,
			    char *buf)
{
	if (list_empty(&journal_list)) {
		sprintf(buf, "Journal is empty:(\n");
	} else {
		struct _journal_head_t *tmp;

		list_for_each_entry(tmp, &journal_list, head) {
			pr_info("%s at %llu jiffies\n", tmp->note,
				tmp->note_jiffies);
		}
		sprintf(buf, "Journal has printed successfuly:)\n");
	}

	return strlen(buf);
}
CLASS_ATTR_CREATE(journal, 0444, &journal_show, NULL);

void cleanup(void)
{
	struct list_head *pos = NULL;
	struct list_head *next;

	list_for_each_safe(pos, next, &journal_list) {
		struct _journal_head_t *tmp =
			list_entry(pos, struct _journal_head_t, head);
		list_del(pos);
		kfree(tmp);
	}

	if (alloc.buff_alloc) {
		kfree(buffer);
		pr_info("Buffer cleared\n");
	}

	if (alloc.dev_create) {
		device_destroy(cl, MKDEV(major_num, 0));
		pr_info("Device destroyed\n");
	}

	if (alloc.buf_clean_attr_create) {
		class_remove_file(cl, CLASS_ATTR(buf_clean));
		pr_info("'buf_clean' attr destroyed\n");
	}

	if (alloc.buffer_size_attr_create) {
		class_remove_file(cl, CLASS_ATTR(buffer_size));
		pr_info("'buffer_size' attr  destroyed\n");
	}

	if (alloc.used_buf_vol_attr_create) {
		class_remove_file(cl, CLASS_ATTR(used_buf_vol));
		pr_info("'used_buf_vol' attr  destroyed\n");
	}

	if (alloc.cl_create) {
		class_destroy(cl);
		pr_info("class destroyed\n");
	}

	if (alloc.reg_cdev) {
		unregister_chrdev(major_num, DEVICE_NAME);
		pr_info("chdev unregistered\n");
	}
}

///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

int __init mod_init(void)
{
	int status = 0;

	alloc.reg_cdev = 0;
	alloc.cl_create = 0;
	alloc.dev_create = 0;
	alloc.buff_alloc = 0;
	alloc.used_buf_vol_attr_create = 0;
	alloc.buffer_size_attr_create = 0;
	alloc.buf_clean_attr_create = 0;
	alloc.journal_attr_create = 0;

	if (buffer_size <= 0) {
		pr_err("Can't create buffer with size: %d bytes:(\n",
		       buffer_size);
		return -EINVAL;
	}

	major_num = register_chrdev(0, DEVICE_NAME, &cdev_fops);
	if (IS_ERR_VALUE(major_num)) {
		pr_err("Can't register cdev:(\n");
		return major_num;
	}
	alloc.reg_cdev = 1;

	cl = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(cl)) {
		pr_err("Can't create class:(\n");
		cleanup();
		return PTR_ERR(cl);
	}
	alloc.cl_create = 1;

	pdev = device_create(cl, NULL, MKDEV(major_num, 0), NULL, "cdev");
	if (IS_ERR(cl)) {
		pr_err("Can't create device:(\n");
		cleanup();
		return PTR_ERR(pdev);
	}
	alloc.dev_create = 1;

	pr_info("Registered device '%s' with %d:%d\n", THIS_MODULE->name,
		major_num, 0);

	// 'buffer_size + 1' for being shure that last symbol is '\0'
	buffer = kzalloc(sizeof(*buffer) * (buffer_size + 1), GFP_KERNEL);
	if (buffer == NULL) {
		pr_err("Can't allocate buffer:(\n");
		cleanup();
		return -ENOMEM;
	}
	alloc.buff_alloc = 1;

	status = class_create_file(cl, CLASS_ATTR(used_buf_vol));
	if (IS_ERR_VALUE(status)) {
		pr_err("Can't create 'used_buf_vol' attr:(\n");
		cleanup();
		return status;
	}
	alloc.used_buf_vol_attr_create = 1;

	status = class_create_file(cl, CLASS_ATTR(buffer_size));
	if (IS_ERR_VALUE(status)) {
		pr_err("Can't create 'buffer_size' attr:(\n");
		cleanup();
		return status;
	}
	alloc.buffer_size_attr_create = 1;

	status = class_create_file(cl, CLASS_ATTR(buf_clean));
	if (IS_ERR_VALUE(status)) {
		pr_err("Can't create 'buf_clean' attr:(\n");
		cleanup();
		return status;
	}
	alloc.buf_clean_attr_create = 1;

	status = class_create_file(cl, CLASS_ATTR(journal));
	if (IS_ERR_VALUE(status)) {
		pr_err("Can't create 'journal' attr:(\n");
		cleanup();
		return status;
	}
	alloc.journal_attr_create = 1;

	pr_info("'%s' module initialized\n", THIS_MODULE->name);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

void __exit mod_cleanup(void)
{
	cleanup();

	// paranoid checking (afterwards to ensure all fops ended)
	if (files_counter != 0)
		pr_err("Some files still opened:("); // should never happen

	pr_info("'%s' module released\n", THIS_MODULE->name);
}

module_init(mod_init);
module_exit(mod_cleanup);
