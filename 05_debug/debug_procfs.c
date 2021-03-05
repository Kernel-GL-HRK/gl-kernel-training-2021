// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "debug_procfs.h"

/* procfs nodes: dir: 'gl_debug', main: 'uppercase', status: 'stat' */
static struct proc_dir_entry *proc_dir, *proc_main, *proc_stat;

/* callbacks prototypes for proc fops initialization */
static ssize_t gl_proc_stat_read(struct file*, char __user*, size_t, loff_t*);
static ssize_t gl_proc_read(struct file*, char __user*, size_t, loff_t*);
static ssize_t gl_proc_write(struct file*, const char __user*, size_t, loff_t*);

/* fops for main node (r/w) */
static const struct file_operations proc_main_ops = {
	.owner = THIS_MODULE,
	.read = gl_proc_read,
	.write = gl_proc_write
};

/* fops for status node (read only) */
static const struct file_operations proc_stat_ops = {
	.owner = THIS_MODULE,
	.read = gl_proc_stat_read
};

static ssize_t gl_proc_stat_read(struct file *f, char __user *buf, size_t len,
								 loff_t *offset)
{
	int _len;
	static char _stat[GL_PROCFS_STAT_BUF_LEN];

	if (*offset > 0)
		return 0;

	/* get stat from comm module */
	_len = gl_comm_stat_read(GL_PROCFS, _stat, sizeof(_stat));
	if (_len < 0)
		return _len;

	/* fill the buffer, return the buffer size */
	if (copy_to_user(buf, _stat, _len))
		return -EFAULT;

	return _len;
}

static ssize_t gl_proc_read(struct file *f, char __user *buf, size_t len,
							loff_t *offset)
{
	ssize_t _len;
	static char _buf[GL_PROCFS_READ_BUF_LEN];

	if (*offset > 0)
		return 0;

	/* get string from comm module */
	_len = gl_comm_main_read(GL_PROCFS, _buf, GL_PROCFS_READ_BUF_LEN);
	if (_len < 0)
		return _len;

	/* fill the buffer, return the buffer size */
	if (copy_to_user(buf, _buf, _len))
		return -EFAULT;

	return _len;
}

static ssize_t gl_proc_write(struct file *f, const char __user *buf, size_t len,
							 loff_t *offset)
{
	static char _buf[GL_COMM_MAX_LIST_PAYLOAD_SIZE];

	if (!len)
		return 0;

	/* check input buffer size, truncate if not fit */
	if (len > GL_COMM_MAX_LIST_PAYLOAD_SIZE) {
		pr_warn("String is too long, truncating to %d byte(s)...\n",
				GL_COMM_MAX_LIST_PAYLOAD_SIZE);
		len = GL_COMM_MAX_LIST_PAYLOAD_SIZE;
	}

	/* copy data to buffer from userspace */
	if (copy_from_user(_buf, buf, len))
		return -EFAULT;

	/* write data to comm module */
	return gl_comm_main_write(GL_PROCFS, _buf, len);
}

int gl_procfs_init(void)
{
	int retval;

	proc_dir = proc_mkdir(GL_PROCFS_DIR, NULL);
	if (!proc_dir) {
		pr_err("Could not initialize /proc/%s!\n", GL_PROCFS_DIR);
		return -ENOMEM;
	}

	proc_main = proc_create(GL_PROCFS_MAIN, 0, proc_dir, &proc_main_ops);
	if (!proc_main) {
		pr_err("Could not initialize /proc/%s/%s!\n",
			GL_PROCFS_DIR, GL_PROCFS_MAIN);
		retval = -ENOMEM;
		goto no_procmain;
	}

	proc_stat = proc_create(GL_PROCFS_STAT, 0, proc_dir, &proc_stat_ops);
	if (!proc_stat) {
		pr_err("Could not initialize /proc/%s/%s!\n",
				GL_PROCFS_DIR, GL_PROCFS_STAT);
		retval = -ENOMEM;
		goto no_procstat;
	}

	return 0;

no_procstat:
	remove_proc_entry(GL_PROCFS_MAIN, proc_dir);
no_procmain:
	remove_proc_entry(GL_PROCFS_DIR, NULL);

	return retval;
}

void gl_procfs_deinit(void)
{
	remove_proc_entry(GL_PROCFS_STAT, proc_dir);
	remove_proc_entry(GL_PROCFS_MAIN, proc_dir);
	remove_proc_entry(GL_PROCFS_DIR, NULL);
}
