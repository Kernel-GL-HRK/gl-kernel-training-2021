// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Nazar Sopiha  <nazar.sopiha@globallogic.com>");
MODULE_DESCRIPTION("The simplest Linux module.");
MODULE_VERSION("1.0");

static int param = 0;


module_param(param, int, 0644);
MODULE_PARM_DESC(param, "Test Parameter, integer type ");


static int __init simple_module_init(void)
{
	if (param) {
		pr_err("Simple module init error - %d\n", param);
		return -1;
	}

	pr_info("Simple module init success!\n");
	return 0;
}

static void __exit simple_module_exit(void)
{
	pr_info("Simple module exit!\n");
}

module_init(simple_module_init);
module_exit(simple_module_exit);
