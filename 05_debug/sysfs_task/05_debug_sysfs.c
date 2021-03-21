// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/string.h>

#define MAX_STR_LEN    120
#define KOBJECT_STR    "conv_to_lowercase"

struct smbl_count {
unsigned int smbl_count;
unsigned int conv_smbl_count;
unsigned int all_smbl_count;
};


MODULE_AUTHOR("Ihor Stepukh");
MODULE_DESCRIPTION("Lowercase converter using sysfs");
MODULE_LICENSE("Dual BSD/GPL");

static char str[MAX_STR_LEN] = "";

static struct smbl_count result;
static struct kobject *conv_to_lowercase;

static ssize_t read_data(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", str);
}


static ssize_t write_data(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int i = 0;

	result.smbl_count = sscanf(buf, "%s", str);
	if (result.smbl_count < 0) {
		pr_info("Incorrect data");
		return -EIO;
	}

	while (str[i] != '\0') {
		// if character is in lowercase then subtract 32
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] = str[i] + 32;

		i++;
	}

	result.smbl_count = count - 1;
	result.conv_smbl_count = result.smbl_count;

	result.all_smbl_count += result.smbl_count;

	pr_info("Please look at input_data");

	return count;
}

static ssize_t read_statistics(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "Statistics:\n"
		"\t%u - Latest entered symbols\n"
		"\t%u - Latest converted symbols\n"
		"\t%u - All entered symbols\n",
		result.smbl_count, result.conv_smbl_count,
		result.all_smbl_count);
}

static struct kobj_attribute result_attr =
	__ATTR(input_data, 0660, read_data, write_data);

static struct kobj_attribute stat_attr =
	__ATTR(stat_data, 0660, read_statistics, NULL);

static struct attribute *conv_attrs[] = {
	&result_attr.attr,
	&stat_attr.attr,
	NULL,
};

static struct attribute_group conv_attr_group = {
	.attrs = conv_attrs
};

static int __init module_05_init(void)
{
	int ret = 0;

	conv_to_lowercase = kobject_create_and_add(KOBJECT_STR, kernel_kobj);
	if (!conv_to_lowercase)
		ret = -ENOMEM;

	ret = sysfs_create_group(conv_to_lowercase, &conv_attr_group);

	pr_info("LowerCase converter module starts\n");

	return ret;
}

static void __exit module_05_exit(void)
{
	kobject_put(conv_to_lowercase);

	pr_info("LowerCase converter module finishes\n");
}

module_init(module_05_init);
module_exit(module_05_exit);

