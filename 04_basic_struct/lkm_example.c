// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_AUTHOR("Ivan Kryvosheia");
MODULE_DESCRIPTION("Test module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.01");

struct my_container {
	struct list_head list;
	int some_member;
};

struct my_obj {
	struct list_head node;
	char *dyn_data;
};


static ssize_t procamp_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf);
static ssize_t procamp_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count);

static int init_sysfs_attr(void);
static void deinit_sysfs_attr(void);

struct kobject *kobj_inst;

static struct kobj_attribute procamp_attr = {
	.attr = { .name = "list", .mode = 0666 },
	.show	= procamp_show,
	.store	= procamp_store
};
struct my_container container;

static int init_sysfs_attr(void)
{
	int rc = 0;

	//create a directory into the /sys/kernel directory
	kobj_inst = kobject_create_and_add("gl_procamp", kernel_kobj);

	if (!kobj_inst)
		return -1;

	rc = sysfs_create_file(kobj_inst, &procamp_attr.attr);

	if (rc) {
		pr_debug("failed to create the foo file in /sys/kernel/kobject_example\n");
		deinit_sysfs_attr();
		return -1;
	}

	return rc;
}

static void deinit_sysfs_attr(void)
{
	sysfs_remove_file(kobj_inst, &procamp_attr.attr);

	if (kobj_inst)
		kobject_put(kobj_inst);
}

static int __init lkm_example_init(void)
{
	int rc = 0;

	rc = init_sysfs_attr();
	if (rc) {
		pr_err("sysfs not initialized\n");
		return -1;
	}

	pr_info("ProCamp Module inited\n");

	INIT_LIST_HEAD(&container.list);

	return rc;
}

void free_data_list(void)
{
	struct my_obj *obj, *temp;

	list_for_each_entry_safe(obj, temp, &container.list, node) {
		list_del(&obj->node);
		kfree(obj->dyn_data);
		kfree(obj);
	}
}

static void __exit lkm_example_exit(void)
{
	deinit_sysfs_attr();

	free_data_list();

	pr_info("ProCamp Module removed\n");
}

ssize_t procamp_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct list_head *i;

	list_for_each(i, &container.list) {
		struct my_obj *obj = list_entry(i, struct my_obj, node);

		if (obj != NULL)
			pr_info("%s", obj->dyn_data);
	}

	return 0;

}

ssize_t procamp_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	int len = strlen(buf);
	struct my_obj *obj;

	if (len <= 0)
		return -1;
	pr_debug("storing %s len %d\n", buf, len);

	obj = kmalloc(1 * sizeof(struct my_obj), GFP_KERNEL);

	if (!obj)
		return -1;

	obj->dyn_data = kstrdup(buf, GFP_KERNEL);
	if (!obj->dyn_data)
		return -1;

	list_add_tail(&obj->node, &container.list);

	return count;
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
