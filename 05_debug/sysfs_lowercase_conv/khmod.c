// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ctype.h>

MODULE_DESCRIPTION("Sysfs lowercase converter");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static struct class *attr_class;
static char message[PAGE_SIZE] = "initial\n";

static const char* string_to_lower(const char *data)
{
	static char result[PAGE_SIZE];
	int i;
	
	strcpy(result, data);

	for(i = 0; result[i]; i++) {
		result[i] = tolower(result[i]);
	}

	return result;
}

static ssize_t rw_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	strcpy(buf, message);
	return strlen(buf);
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr,
				const char *buf, size_t count)
{
	size_t len = strnlen(buf, count);

	if (len != count) {
		pr_err_once("Error[%s]: length error!", __func__);
		return -EINVAL;
	}
	
	strcpy(message, string_to_lower(buf));
	return count;
}

struct class_attribute class_attr_rw = {
	.attr = { .name = "rw", .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

static int __init firstmod_init(void)
{
	int ret = 0
;
	attr_class = class_create(THIS_MODULE, "kharchuk");

	if (attr_class == NULL) {
		pr_err("%s: error creating sysfs class\n", THIS_MODULE->name);
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_rw);
	if (ret) {
		pr_err("%s: error creating sysfs class rw attribute\n",
			THIS_MODULE->name);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("%s: loaded!\n", THIS_MODULE->name);

	return 0;
}

static void __exit firstmod_exit(void)
{

	class_remove_file(attr_class, &class_attr_rw);
	class_destroy(attr_class);

	pr_info("%s: removed!\n", THIS_MODULE->name);
}

module_init(firstmod_init);
module_exit(firstmod_exit);

