// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

static int foo;

module_param(foo, int, 0644);
MODULE_PARM_DESC(foo, "An integer");

static int __init simple_module_init(void)
{
	if (foo) {
		pr_err("Simple module init error - %d\n", foo);
		return -1;
	}

	pr_info("Simple module init success!\n");
	return 0;
}

static void __exit simple_module_exit(void)
{
	pr_info("Simple module release!\n");
}

module_init(simple_module_init);
module_exit(simple_module_exit);
