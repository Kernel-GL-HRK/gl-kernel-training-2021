// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

#include <linux/sysfs.h>
#include <linux/device.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>

//#define DEBUG

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("List data module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

struct data_node {
	struct list_head data_list;
	size_t data_size;
	const char *data_ptr;
};

struct accum_data {
	struct list_head accum_list;
	size_t accum_size;
} gl_accum;

static int entry_del(struct data_node *entry)
{
	int res = entry->data_size;

	list_del(&entry->data_list);
	kfree(entry->data_ptr);
	kfree(entry);
	return res;
}

static int accum_entry_add(struct accum_data *accum, const char *new_data_prt,
						  size_t new_data_size)
{
	struct data_node *entry;

	if ((accum == NULL) || (new_data_prt == NULL)) {
		pr_err("Linked list of input doesn't exist!\n");
		return -EINVAL;
	}

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (entry == NULL)
		goto exit;

	entry->data_ptr = kstrdup(new_data_prt, GFP_KERNEL);
	if (entry->data_ptr == NULL)
		goto exit;

	entry->data_size = new_data_size;
	accum->accum_size += entry->data_size;
	list_add_tail(&entry->data_list, &accum->accum_list);

	return 0;

exit:
	kzfree(entry);
	return -ENOMEM;
}

static int accum_free_from_head(struct accum_data *aaccum, size_t req_space)
{
	struct data_node *entry = NULL;

	if (aaccum == NULL) {
		pr_err("Linked list doesn't exist!\n");
		return -EINVAL;
	}

	while ((aaccum->accum_size + req_space) > PAGE_SIZE) {

		if (list_empty(&aaccum->accum_list))
			return -EINVAL;

		entry = list_first_entry(&aaccum->accum_list, struct data_node,
								 data_list);
		aaccum->accum_size -= entry_del(entry);
	}

	return 0;
}

static int accum_print(struct accum_data *aaccum, char *buf)
{
	int res = 0;
	int cursor = 0;

	struct data_node *entry = NULL;

	if ((aaccum == NULL) || (buf == NULL)) {
		pr_err("Linked list or output buffer doesn't exist!\n");
		return -EINVAL;
	}

	list_for_each_entry(entry, &aaccum->accum_list, data_list) {
		res = sprintf(&buf[cursor], "%s\n", entry->data_ptr);
		if (res > 0)
			cursor += res;
		else {
			pr_err("Failed to write output.\n");
			return -EIO;
		}
	}

	return cursor;
}

static int accum_free(struct accum_data *aaccum)
{
	struct data_node *entry = NULL,
					 *tmp = NULL;

	if (aaccum == NULL) {
		pr_err("Linked list doesn't exist!\n");
		return -EINVAL;
	}

	list_for_each_entry_safe(entry, tmp, &aaccum->accum_list, data_list) {
		aaccum->accum_size -= entry_del(entry);
	}
	return 0;
}

static ssize_t accum_show(struct kobject *kobj, struct kobj_attribute *attr,
					   char *buf)
{
	return accum_print(&gl_accum, buf);
}

static ssize_t accum_store(struct kobject *kobj, struct kobj_attribute *attr,
						const char *buf, size_t count)
{
	int res = 0;

	res = accum_free_from_head(&gl_accum, count);

	if (res != 0)
		return res;

	res = accum_entry_add(&gl_accum, buf, count);

	if (res != 0)
		return res;

	return count;
}

static struct kobject *hello_kobject;
struct kobj_attribute kobj_attr_list = {
	.attr = { .name = "list", .mode = 0666 },
	.show	= accum_show,
	.store	= accum_store
};

static int init_sysfs(void)
{
	int res;

	hello_kobject = kobject_create_and_add("listed_data", kernel_kobj);
	if (hello_kobject == NULL) {
		pr_err("bts_module: error creating kobject\n");
		return -ENOMEM;
	}

	res = sysfs_create_file(hello_kobject, &kobj_attr_list.attr);
	if (res != 0) {
		pr_err("bts_module: error creating sysfs file\n");
		kobject_del(hello_kobject);
		return res;
	}

	return 0;
}

static int __init lds_module_init(void)
{
	int res = 0;

	INIT_LIST_HEAD(&gl_accum.accum_list);

#ifdef DEBUG
	gl_accum.accum_size = PAGE_SIZE - 10;
#else
	gl_accum.accum_size = 0;
#endif

	pr_info("lds_module: module starting\n");

	res = init_sysfs();
	if (res != 0)
		return res;

	return 0;
}

static void __exit lds_module_exit(void)
{
	if (hello_kobject != NULL) {
		sysfs_remove_file(hello_kobject, &kobj_attr_list.attr);
		kobject_del(hello_kobject);
	}

	accum_free(&gl_accum);
	pr_info("lds_module: module exited\n");
}

module_init(lds_module_init);
module_exit(lds_module_exit);
