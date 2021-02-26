// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_DESCRIPTION("Basic module demo");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static char *user;
module_param(user, charp, 0);

static int __init firstmod_init(void)
{
	if (user == NULL) {
		pr_err("Username needed\n");
		return -EINVAL;
	}

	pr_info("Hello, %s!\n", user);
	return 0;
}

static void __exit firstmod_exit(void)
{
	pr_info("See u, %s!\n", user);
}

module_init(firstmod_init);
module_exit(firstmod_exit);

