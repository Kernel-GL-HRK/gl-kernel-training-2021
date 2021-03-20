#include <linux/init.h>    // used by module_init, module_exit macros
#include <linux/jiffies.h> // where jiffies and its helpers reside
#include <linux/kernel.h>  // required for sysinfo
#include <linux/module.h>  // required by all modules
#include <linux/moduleparam.h>

MODULE_DESCRIPTION("Basic module demo: init, deinit, printk, jiffies");
MODULE_AUTHOR("Anton Kotsiubailo");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

static long count_start;
static char *usrname;

module_param(usrname, charp, 0);
MODULE_PARM_DESC(usrname, "user name");

static int __init firstmod_init(void)
{
  count_start = jiffies;
  char str[] = "$usrname";
  if (!usrname) {
    printk(KERN_WARNING "username wasn`t passed as a parameter\n");
    usrname = str;
  }

  printk(KERN_INFO "Hello, %s!\n", usrname);

  return 0;
}

static void __exit firstmod_exit(void) { printk(KERN_INFO "Goodbye\n"); }

module_init(firstmod_init);
module_exit(firstmod_exit);
