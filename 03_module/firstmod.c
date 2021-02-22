// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>

MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

enum STATUS {
	OK = 0,
	ERROR = -1
};

static int status;
module_param(status, int, 0);

static char *name = "oleh";
module_param(name, charp, 0);

static int __init hello_init(void)
{
	enum STATUS a_status = OK;

	pr_emerg("Hello, %s!\n Status %i\n", name, status);
	if (status >= 0) {
		pr_emerg("Successful start\n");
		a_status = OK;
	} else {
		pr_emerg("Starting failed. Module has not installed.\n");
		a_status = ERROR;
	}
	return a_status;
}

static void __exit hello_exit(void)
{
	pr_emerg("See you soon, %s!\n", name);
}

module_init(hello_init);
module_exit(hello_exit);
