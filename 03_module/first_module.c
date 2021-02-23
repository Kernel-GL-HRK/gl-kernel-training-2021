// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("First module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

#define GREAT_ANSVER 42

static char *user_name;
module_param(user_name, charp, 0);

static int ansver;
module_param(ansver, int, 0);

static int __init first_module_init(void)
{
	if (user_name != NULL)
		pr_warn("Hello, %s!\n", user_name);
	else {
		user_name = "Unknown user";
		pr_warn("Unknown user!\n");
	}

	if (ansver == GREAT_ANSVER)
		pr_warn("%s, you are correct! ansver is: %d",
				user_name,
				ansver);
	else
		pr_warn("%s, you are incorrect!", user_name);

	return 0;
}

static void __exit first_module_exit(void)
{
	pr_warn("Live long and prosper, %s!\n", user_name);
}

module_init(first_module_init);
module_exit(first_module_exit);
