// SPDX-License-Identifier: MIT and GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

MODULE_DESCRIPTION("Sysfs llist demo module");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static char user[PAGE_SIZE] = "test";

static ssize_t rw_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("%s: rw_show: name = %s\n", THIS_MODULE->name, user);

	sprintf(buf, "%s\n", user);
	return strlen(buf);
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%s", user);

	pr_info("%s: rw_store: value = %s\n", THIS_MODULE->name, user);
	return count;
}
struct class_attribute class_attr_rw = {
	.attr = { .name = "rw", .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

static struct class *attr_class = 0;

static int __init firstmod_init(void)
{
	int ret = 0;
	attr_class = class_create(THIS_MODULE, "kharchuk");
	if (attr_class == NULL) {
		pr_err("%s: error creating sysfs class\n", THIS_MODULE->name);
		return -ENOMEM;
	}
	ret = class_create_file(attr_class, &class_attr_rw);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n", THIS_MODULE->name);
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
	pr_info("See u, %s", user);
}

module_init(firstmod_init);
module_exit(firstmod_exit);

