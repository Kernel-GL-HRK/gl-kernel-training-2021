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

static char buf_msg[LEN_MSG + 1] = "Hello from module!\n";

static LIST_HEAD(my_list);

const struct data {

const struct list_head head;
char text[LEN_MSG];

};

const static struct data *tmp;

static ssize_t hello_show(struct class *class,
			  struct class_attribute *attr, char *buf)
{

if (list_empty(&my_list)) {
	sprintf(buf, "List is empty:(\n");
} else {
	struct data *tmp;

	list_for_each_entry(tmp, &my_list, head) {
		pr_info("%s\n", tmp->text);
	}

	sprintf(buf, "List has printed successfuly:)\n");

	}

return strlen(buf);
}

static ssize_t hello_store(struct class *class, struct class_attribute *attr,
			   const char *buf, size_t count)
{
pr_info("write %ld\n", (long)count);
strncpy(buf_msg, buf, count);
buf_msg[count] = '\0';
tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);
strcpy(tmp->text, buf_msg);
list_add_tail(&(tmp->head), &my_list);
pr_info("String was written successfuly\n", buf_msg);

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
