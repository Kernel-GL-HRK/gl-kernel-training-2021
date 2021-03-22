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

struct data {
  struct list_head head;
  char text[LEN_MSG];
};

static struct data *tmp;

static ssize_t hello_show(struct class *class, struct class_attribute *attr,
                      char *buf)
{

  strcpy(buf, buf_msg);
  printk("read %ld\n", (long)strlen(buf));
  return strlen(buf);
}

static ssize_t hello_store(struct class *class, struct class_attribute *attr,
                       const char *buf, size_t count)
{
  static int i;
  for (i = 0; buf_msg[i] != '\0'; i++) {
    if (buf_msg[i] >= 'A' && buf_msg[i] <= 'Z')
      buf_msg[i] = buf_msg[i] + 32;
  }
  printk("write %ld\n", (long)count);
  strncpy(buf_msg, buf, count);
  buf_msg[count] = '\0';
  return count;
}

#define CLASS_ATTR(_name, _mode, _show, _store)                                \
  struct class_attribute class_attr_##_name =                                  \
      __ATTR(_name, _mode, _show, _store)

CLASS_ATTR(hello, (S_IWUSR | S_IRUGO), &hello_show, &hello_store);

static struct class *hello_class;

int __init hello_init(void)
{
  int res;
  hello_class = class_create(THIS_MODULE, "hello-class");
  if (IS_ERR(hello_class))
    printk("bad class create\n");
  res = class_create_file(hello_class, &class_attr_hello);
  printk("'hello' module initialized\n");
  return 0;
}

void hello_cleanup(void)
{
  struct list_head *pos = NULL;

  list_for_each(pos, &my_list) {
    struct data *tmp = list_entry(pos, struct data, head);
    list_del(pos);
    kfree(tmp);
  }
  class_remove_file(hello_class, &class_attr_hello);
  class_destroy(hello_class);
  return;
}

module_init(hello_init);
module_exit(hello_cleanup);
