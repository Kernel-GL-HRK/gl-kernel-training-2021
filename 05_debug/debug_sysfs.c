// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "debug_sysfs.h"

/* sysfs kernel node */
static struct kobject *sysfs_kobj;

/* callbacks prototypes for proc fops initialization */
static ssize_t gl_sys_stat_read(struct kobject*, struct kobj_attribute*, char*);
static ssize_t gl_sys_read(struct kobject*, struct kobj_attribute *, char*);
static ssize_t gl_sys_write(struct kobject*, struct kobj_attribute*,
							const char*, size_t);

/* sysfs attribute for main: 'lowercase' node */
static struct kobj_attribute main_attr =
		__ATTR(GL_SYSFS_MAIN, 0664, gl_sys_read, gl_sys_write);

/* sysfs attribute for status: 'stat' node */
static struct kobj_attribute stat_attr =
		__ATTR(GL_SYSFS_STAT, 0664, gl_sys_stat_read, NULL);

static struct attribute *attrs[] = {
		&main_attr.attr,
		&stat_attr.attr,
		NULL,   /* need to NULL terminate the list of attributes */
};

const struct attribute_group attr_group = {
		.attrs = attrs
};

static ssize_t gl_sys_stat_read(struct kobject *kobj,
						struct kobj_attribute *attr,
						char *buf)
{
	/* get stat from comm module */
	return gl_comm_stat_read(GL_SYSFS, buf, GL_SYSFS_STAT_BUF_LEN);
}

static ssize_t gl_sys_read(struct kobject *kobj,
						struct kobj_attribute *attr,
						char *buf)
{
	/* get string from comm module */
	return gl_comm_main_read(GL_SYSFS, buf, GL_COMM_MAX_LIST_PAYLOAD_SIZE);
}

static ssize_t gl_sys_write(struct kobject *kobj,
						struct kobj_attribute *attr,
						const char *buf, size_t len)
{
	if (!len)
		return 0;

	/* check input buffer size, truncate if not fit */
	if (len > GL_COMM_MAX_LIST_PAYLOAD_SIZE) {
		pr_warn("String is too long, truncating to %d byte(s)...\n",
				GL_COMM_MAX_LIST_PAYLOAD_SIZE);
		len = GL_COMM_MAX_LIST_PAYLOAD_SIZE;
	}

	/* write data to comm module */
	return gl_comm_main_write(GL_SYSFS, buf, len);
}

int gl_sysfs_init(void)
{
	int retval;

	sysfs_kobj = kobject_create_and_add(GL_SYSFS_DIR, kernel_kobj);
	if (!sysfs_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(sysfs_kobj, &attr_group);
	if (retval)
		kobject_put(sysfs_kobj);

	return 0;
}

void gl_sysfs_deinit(void)
{
	kobject_put(sysfs_kobj);
}
