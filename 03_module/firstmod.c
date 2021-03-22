// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

MODULE_DESCRIPTION("Basic module demo: init, deinit, printk");
MODULE_AUTHOR("Anton Kotsiubailo");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL-2.0"); // this affects the kernel behavior

static char *usrname;

module_param(usrname, charp, 0);
MODULE_PARM_DESC(usrname, "user name");

static int __init firstmod_init(void)
{

if (!usrname) {
	pr_warn("username wasn`t passed as a parameter\n");
	return -EINVAL;
}

pr_warn("Hello, %s!\n", usrname);

return 0;
}

static void __exit firstmod_exit(void)
{
pr_info("Goodbye\n");
}

module_init(firstmod_init);
module_exit(firstmod_exit);
