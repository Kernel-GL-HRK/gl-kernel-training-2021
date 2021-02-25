// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("First module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

#define GREAT_ANSWER 42

static char *user_name;
module_param(user_name, charp, 0);
MODULE_PARM_DESC(user_name, "User nane");

static int answer;
module_param(answer, int, 0);
MODULE_PARM_DESC(answer, "User answer to the Ultimate Question");

static int __init first_module_init(void)
{
	if (user_name != NULL)
		pr_warn("Hello, %s!\n", user_name);
	else
		return -EINVAL;


	if (answer == GREAT_ANSWER)
		pr_warn("%s, you are correct! answer is: %d",
				user_name,
				answer);
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
