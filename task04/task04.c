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

struct data {
  struct list_head head;
  char text[LEN_MSG];
};

LIST_HEAD(my_list);
static struct data *tmp;

#define IOFUNCS(name)                                                          \
  static char buf_##name[LEN_MSG + 1] = "not initialized " #name "\n";         \
  static ssize_t SHOW_##name(struct class *class,                              \
			     struct class_attribute *attr, char *buf)          \
{        								       \
    strcpy(buf, buf_##name);                                                   \
    printk("read %ld\n", (long)strlen(buf));                                   \
    if (list_empty(&my_list)) {                                                \
      sprintf(buf, "List is empty:(\n");                                       \
    } else {                                                                   \
      struct data *tmp;                                                        \
      list_for_each_entry(tmp, &my_list, head) { pr_info("%s\n", tmp->text); } \
      sprintf(buf, "List has printed successfuly:)\n");                        \
    }                                                                          \
    return strlen(buf);                                                        \
  }                                                                            \
  static ssize_t STORE_##name(struct class *class,                             \
			      struct class_attribute *attr, const char *buf,   \
			      size_t count) 				       \
{                                  					       \
    printk("write %ld\n", (long)count);                                        \
    strncpy(buf_##name, buf, count);                                           \
    buf_##name[count] = '\0';                                                  \
    tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);                                   \
    strcpy(tmp->text, buf_##name);                                             \
    list_add_tail(&(tmp->head), &my_list);                                     \
    printk("String was wrote succesfully\n");                                  \
    return count;                                                              \
  }

IOFUNCS(hello);

#define OWN_CLASS_ATTR(name)                                                   \
  struct class_attribute class_attr_##name =                                   \
      __ATTR(name, (S_IWUSR | S_IRUGO), &SHOW_##name, &STORE_##name)

static OWN_CLASS_ATTR(hello);

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
