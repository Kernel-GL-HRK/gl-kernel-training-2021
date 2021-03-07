// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ihor Stepukh");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

static char *name = "user";
module_param(name, charp, 0444); //just can be read
MODULE_PARM_DESC(name, "The name to display");  ///< parameter description

static int __init module_03_init(void)
{
	if (name == NULL) {
		pr_warn("Incorrect name!\n");
		return -EINVAL;
	}

	pr_info("Hello, %s!\n", name);

	return 0;
}

static void __exit module_03_exit(void)
{
	pr_info("Goodbye, %s!\n", name);
}

module_init(module_03_init);
module_exit(module_03_exit);
