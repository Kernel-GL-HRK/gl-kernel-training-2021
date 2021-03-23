// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "gl_procfs.h"

#define GL_PROCFS_READ_BUF_SIZE 16

/* procfs nodes: dir: 'gl_cdev', size: 'buffer_size', volume: 'buffer_volume' */
static struct proc_dir_entry *proc_dir, *proc_size, *proc_volume;

/* procfs payload */
static size_t buffer_size;
static size_t *buffer_volume;

/* callbacks prototypes for proc fops initialization */
static ssize_t gl_proc_size_read(struct file*, char __user*, size_t, loff_t*);
static ssize_t gl_proc_volume_read(struct file*, char __user*, size_t, loff_t*);

/* fops for main node (read only) */
static const struct file_operations proc_size_ops = {
	.owner = THIS_MODULE,
	.read = gl_proc_size_read,
};

/* fops for status node (read only) */
static const struct file_operations proc_volume_ops = {
	.owner = THIS_MODULE,
	.read = gl_proc_volume_read
};

static ssize_t gl_proc_size_read(struct file *f, char __user *buf, size_t len,
	loff_t *offset)
{
	int _len;
	static char _stat[GL_PROCFS_READ_BUF_SIZE];

	if (*offset > 0)
		return 0;

	_len = sprintf(_stat, "%u\n", buffer_size);

	/* fill the buffer, return the buffer size */
	if (copy_to_user(buf, _stat, _len))
		return -EFAULT;

	return _len;
}

static ssize_t gl_proc_volume_read(struct file *f, char __user *buf, size_t len,
	loff_t *offset)
{
	int _len;
	static char _stat[GL_PROCFS_READ_BUF_SIZE];

	if (*offset > 0)
		return 0;

	_len = sprintf(_stat, "%u\n", *buffer_volume);

	/* fill the buffer, return the buffer size */
	if (copy_to_user(buf, _stat, _len))
		return -EFAULT;

	return _len;
}

int gl_procfs_init(size_t size, size_t *volume)
{
	int retval;

	buffer_size = size;
	buffer_volume = volume;

	proc_dir = proc_mkdir(GL_PROCFS_DIR, NULL);
	if (!proc_dir) {
		pr_err("GL Could not initialize /proc/%s!\n", GL_PROCFS_DIR);
		return -ENOMEM;
	}

	proc_size = proc_create(GL_PROCFS_BUF_SIZE, 0, proc_dir,
		&proc_size_ops);
	if (!proc_size) {
		pr_err("GL Could not initialize /proc/%s/%s!\n",
			GL_PROCFS_DIR, GL_PROCFS_BUF_SIZE);
		retval = -ENOMEM;
		goto no_procsize;
	}

	proc_volume = proc_create(GL_PROCFS_BUF_VOL, 0, proc_dir,
		&proc_volume_ops);
	if (!proc_volume) {
		pr_err("GL Could not initialize /proc/%s/%s!\n",
				GL_PROCFS_DIR, GL_PROCFS_BUF_VOL);
		retval = -ENOMEM;
		goto no_procvol;
	}

	return 0;

no_procvol:
	remove_proc_entry(GL_PROCFS_BUF_SIZE, proc_dir);
no_procsize:
	remove_proc_entry(GL_PROCFS_DIR, NULL);

	return retval;
}

void gl_procfs_deinit(void)
{
	remove_proc_entry(GL_PROCFS_BUF_VOL, proc_dir);
	remove_proc_entry(GL_PROCFS_BUF_SIZE, proc_dir);
	remove_proc_entry(GL_PROCFS_DIR, NULL);
}
