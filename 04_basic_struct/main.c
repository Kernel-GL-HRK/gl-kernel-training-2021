#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

#include <linux/sysfs.h>
#include <linux/device.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>

static struct kobject *hello;

static LIST_HEAD(list);

struct hello_node {
	struct list_head head;
	char *str;
};

static ssize_t rw_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	struct hello_node *item = NULL;
	int offset = 0;
	int state = 0;

	list_for_each_entry(item, &list, head) {
		pr_info("Try to read some info\n");
		state = sprintf(buf + offset, "%s",item->str);
		if (state < 0)
		{
			pr_info("Error\n");
			return state;
		}
		pr_info("Success\n");
		offset += state;
	}
	return offset;
}

static ssize_t rw_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct hello_node *item = kzalloc(sizeof(*item), GFP_KERNEL);

	item->str = kzalloc(sizeof(char) * 10 , GFP_KERNEL);
	strcpy(item->str, buf);
	list_add(&item->head, &list);
	pr_info("New item in list %s", buf);
	return count;
}

static struct kobj_attribute attr =
	__ATTR(list, 0660, rw_show, rw_store);

static int mymodule_init(void)
{
	int state = 0;

	pr_info("Module initialized successfully \n");
	hello = kobject_create_and_add("kobject_list", kernel_kobj);
	(!hello) ? state++ : state;
	state = sysfs_create_file(hello, &attr.attr);
	return state;
}

static void mymodule_exit(void)
{
	pr_info("Task 04. Done!\n");
	kobject_put(hello);
	sysfs_remove_file(hello, &attr.attr);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Vladyslav Andrishko <v.andrishko.v333@gmail.com>");
MODULE_DESCRIPTION("Kernel module example with list structure");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
