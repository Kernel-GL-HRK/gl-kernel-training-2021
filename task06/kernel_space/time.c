#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/ktime.h>

MODULE_DESCRIPTION("Basic module demo: work with time");
MODULE_AUTHOR("Anton KOtsiubailo");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define LEN_MSG 160
#define CLASS_ATTR(_name, _mode, _show, _store)                                \
  struct class_attribute class_attr_##_name =                                  \
      __ATTR(_name, _mode, _show, _store)

static char buf_msg[LEN_MSG + 1] = "Hello from module!\n";
static ktime_t currtime;
static ktime_t pasttime;
static long prev = 0;

static ssize_t x_show(struct class *class, struct class_attribute *attr,
                      char *buf)
{
  struct timespec cur, past;
  currtime = ktime_get();
  cur = ktime_to_timespec(currtime);
  past = ktime_to_timespec(pasttime);
  strcpy(buf, buf_msg);
  printk("read %ld\n", (long)strlen(buf));
  printk("time: %ld\n", cur.tv_nsec - past.tv_nsec);
  printk("previous time: %ld\n", prev);
  prev = cur.tv_nsec - past.tv_nsec;
  pasttime = currtime;
  return strlen(buf);
}


CLASS_ATTR(xxx, (S_IWUSR | S_IRUGO), &x_show, NULL);

static struct class *x_class;

int __init x_init(void)
{
  int res;
  x_class = class_create(THIS_MODULE, "x-class");
  if (IS_ERR(x_class))
    printk("bad class create\n");
  res = class_create_file(x_class, &class_attr_xxx);
  printk("'xxx' module initialized\n");
  return res;
}

void x_cleanup(void)
{

  class_remove_file(x_class, &class_attr_xxx);
  class_destroy(x_class);
  return;
}

module_init(x_init);
module_exit(x_cleanup);
