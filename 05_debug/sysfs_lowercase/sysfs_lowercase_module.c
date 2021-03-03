// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/list.h>


MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Simple string lowercase converter using sysfs");
MODULE_LICENSE("Dual BSD/GPL");


static struct kobject *lc_converter;
static char string[PAGE_SIZE] = "";

/* Statistic vars */
static int last_symbol_cnt;
static int total_symbol_cnt;
static int last_conv_symbol_cnt;
static int total_conv_symbol_cnt;


static ssize_t sysfs_lowercase_module_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", string);
}


static ssize_t sysfs_lowercase_module_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int i = 0;

	last_symbol_cnt = sscanf(buf, "%s", string);
	if (last_symbol_cnt < 0) {
		pr_info("Wrong string");

		return count;
	}

	for (i = 0; string[i] != '\0'; i++) {
		if ((string[i] >= 'A') && (string[i] <= 'Z')) {
			string[i] += 'a' - 'A';
			last_conv_symbol_cnt++;
		}
	}

	last_symbol_cnt = count - 1;

	total_symbol_cnt += last_symbol_cnt;

	total_conv_symbol_cnt += last_conv_symbol_cnt;

	pr_info("To get converted result print: cat text");

	return count;
}


static ssize_t sysfs_lowercase_module_statistics(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "sysfs_lowercase_module statistic:\n"
						"\t%d - symbols entered during last operation\n"
						"\t%d - symbols converted during last operation\n"
						"\t%d - symbols entered since module insertion\n"
						"\t%d - symbols converted since module insertion\n",
			last_symbol_cnt, last_conv_symbol_cnt,
			total_symbol_cnt, total_conv_symbol_cnt);
}


static struct kobj_attribute lc_result_attribute =
	__ATTR(text, 0660, sysfs_lowercase_module_show,
		   sysfs_lowercase_module_store);


static struct kobj_attribute lc_statistic_attribute =
	__ATTR(statistic, 0660, sysfs_lowercase_module_statistics, NULL);


static struct attribute *lc_conveter_attrs[] = {
	&lc_result_attribute.attr,
	&lc_statistic_attribute.attr,
	NULL,
};


static struct attribute_group lc_converter_attr_group = {
	.attrs = lc_conveter_attrs
};

static int __init sysfs_lowercase_module_init(void)
{
	int ret_value = 0;

	lc_converter = kobject_create_and_add("lc_converter", kernel_kobj);
	if (!lc_converter)
		ret_value = -ENOMEM;

	ret_value = sysfs_create_group(lc_converter, &lc_converter_attr_group);

	pr_info("sysfs_lowercase_module says: Hello!\n");

	return ret_value;
}



static void __exit sysfs_lowercase_module_exit(void)
{
	kobject_put(lc_converter);

	pr_info("sysfs_lowercase_module says: Bye!\n");
}

module_init(sysfs_lowercase_module_init);
module_exit(sysfs_lowercase_module_exit);
