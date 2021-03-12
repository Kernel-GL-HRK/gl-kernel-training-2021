// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/printk.h>

MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Debug info in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

#define DIR_NAME	"debug_info_dir"
#define ENT_LOWER_NAME	"lowercase"
#define ENT_STAT_NAME	"info"

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
				char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,
				const char *buf, size_t count);
static ssize_t sysfs_read_info(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf);

static char buffer[PAGE_SIZE];
static int num_calls_read;
static int num_calls_write;
static int num_char_processed;
static int num_char_converted;

static struct kobject *sysfs_ent;
struct kobj_attribute kobj_attr_sysfs = {
	.attr = { .name = ENT_LOWER_NAME, .mode = 0666 },
	.show	= sysfs_show,
	.store	= sysfs_store
};
struct kobj_attribute kobj_attr_info = {
	.attr = { .name = ENT_STAT_NAME, .mode = 0444 },
	.show	= sysfs_read_info
};

static int test_sysfs_init(void)
{
	int res = 0;

	sysfs_ent = kobject_create_and_add(DIR_NAME, kernel_kobj);
	if (sysfs_ent == NULL) {
		pr_err("debug_module: error! cannot create sysfs kobject.\n");
		return -ENOMEM;
	}

	res = sysfs_create_file(sysfs_ent, &kobj_attr_sysfs.attr);
	if (res)
		goto error;

	res = sysfs_create_file(sysfs_ent, &kobj_attr_info.attr);
	if (res)
		goto error;

	pr_info("=== debug_module loaded ===\n");
	return 0;

error:
	pr_err("debug_module: error! cannot create sysfs class attribute\n");
	kobject_del(sysfs_ent);
	return res;
}

static void test_sysfs_exit(void)
{
	kobject_del(sysfs_ent);
	pr_info("--- debug_module removed ---\n");
}

ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	num_calls_read++;
	return sprintf(buf, buffer);
}

ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	int i = 0;

	num_calls_write++;
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, buf, count);
	buffer[count] = '\0';

	if (count < 0) {
		pr_err("debug_module: error! can't get user data.\n");
		return count;
	}
	pr_info("passed data: %s\n", buffer);

	while (buffer[i] != 0) {
		if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
			buffer[i] = buffer[i] + 32;
			num_char_converted++;
		}
		++i;
	}

	pr_info("data in lowercase: %s", buffer);
	num_char_processed += count - 1;

	return count;
}

ssize_t sysfs_read_info(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	int offset;

	offset = sprintf(buf, "Lowercase conversion stat info:\n");
	offset += sprintf(buf + offset,
		"Total calls to read string: %d\n", num_calls_read);
	offset += sprintf(buf + offset,
		"Total calls to write string: %d\n", num_calls_write);
	offset += sprintf(buf + offset,
		"Total characters processed: %d\n", num_char_processed);
	offset += sprintf(buf + offset,
		"Total characters converted: %d\n", num_char_converted);

	return offset;
}

module_init(test_sysfs_init);
module_exit(test_sysfs_exit);
