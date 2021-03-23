// SPDX-License-Identifier: GPL-2.0
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/string.h>
#include <linux/version.h>

MODULE_DESCRIPTION("Basic module demo: sysfs, kobj, linked_list");
MODULE_AUTHOR("Anton KOtsiubailo");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define LEN_MSG 160

static LIST_HEAD(my_list);

const struct data {

const struct list_head head;
char text[LEN_MSG];

};

const static struct data *tmp;

static ssize_t hello_show(struct class *class,
			  struct class_attribute *attr, char *buf)
{

int x;
int n;

if (list_empty(&my_list)) {
	pr_info("List is empty:(\n");
} else {
	struct data *tmp;

	x = 0;
	n = 0;
	list_for_each_entry(tmp, &my_list, head) {

		n = snprintf(buf + x, PAGE_SIZE - x, "%s\n", tmp->text);
		if (n < 0)
			return -EINVAL;
		x = x + n;
	}

	pr_info("List has printed successfuly:)\n");

	}

return strlen(buf);
}

static ssize_t hello_store(struct class *class, struct class_attribute *attr,
			   const char *buf, size_t count)
{

if (count > LEN_MSG) {
	pr_err("Entered string is too long\n");
	return -EINVAL;
}
pr_info("write %ld\n", (long)count);
tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);
strncpy(tmp->text, buf, LEN_MSG);
list_add_tail(&(tmp->head), &my_list);
pr_info("String was written successfuly\n");

return count;
}

#define CLASS_ATTR(_name, _mode, _show, _store)                                \
struct class_attribute class_attr_##_name =                              \
__ATTR(_name, _mode, _show, _store)

CLASS_ATTR(hello, 0644, &hello_show, &hello_store);

const static struct class *hello_class;

int __init hello_init(void)
{
int res;
hello_class = class_create(THIS_MODULE, "hello-class");
if (IS_ERR(hello_class))
	pr_err("bad class create\n");
res = class_create_file(hello_class, &class_attr_hello);
pr_info("'hello' module initialized\n");
return 0;
}

void hello_cleanup(void)
{
const struct list_head *pos;

list_for_each(pos, &my_list) {
	const struct data *tmp = list_entry(pos, struct data, head);

	list_del(pos);
	kfree(tmp);
}
class_remove_file(hello_class, &class_attr_hello);
class_destroy(hello_class);
return;
}

module_init(hello_init);
module_exit(hello_cleanup);
