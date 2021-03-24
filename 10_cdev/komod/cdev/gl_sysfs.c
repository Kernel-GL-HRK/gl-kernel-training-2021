// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "gl_sysfs.h"

/* sysfs kernel node */
static struct kobject *sysfs_kobj;
static size_t *read_bytes;
static size_t *written_bytes;
static size_t clean_count;

/* callbacks prototypes for sys attributes */
static ssize_t gl_sys_clean_write(struct kobject *, struct kobj_attribute *,
	const char *, size_t);
static ssize_t gl_sys_debug_read(struct kobject *, struct kobj_attribute *,
	char *);

/* sysfs attribute for main: 'lowercase' node */
static struct kobj_attribute clean_attr =
		__ATTR(GL_SYSFS_CLEAN, 0664, NULL, gl_sys_clean_write);

/* sysfs attribute for status: 'stat' node */
static struct kobj_attribute debug_attr =
		__ATTR(GL_SYSFS_DEBUG, 0664, gl_sys_debug_read, NULL);

static struct attribute *attrs[] = {
		&clean_attr.attr,
		&debug_attr.attr,
		NULL,   /* need to NULL terminate the list of attributes */
};

const struct attribute_group attr_group = {
		.attrs = attrs
};

static ssize_t gl_sys_clean_write(struct kobject *kobj,
	struct kobj_attribute *kattr, const char *buf, size_t len)
{
	gl_clean_buffer();
	clean_count++;

	return len;
}

static ssize_t gl_sys_debug_read(struct kobject *kobj,
	struct kobj_attribute *kattr, char *buf)
{
	int _len;

	_len = sprintf(buf,
		"bytes_read\t%u\nbytes_written\t%u\nclean_count\t%u\n",
			*read_bytes, *written_bytes, clean_count);

	return _len;
}

int gl_sysfs_init(size_t *read, size_t *written)
{
	int retval;

	read_bytes = read;
	written_bytes = written;

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
