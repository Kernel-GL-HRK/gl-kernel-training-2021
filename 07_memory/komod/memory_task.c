// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 07_memory task
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>

static int __init gl_memory_init(void)
{
	pr_info("GL Kernel Training 2021\n");

	return 0;
}

static void __exit gl_memory_exit(void)
{
	pr_info("Exit from 07_memory module\n");
}

module_init(gl_memory_init);
module_exit(gl_memory_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 07_memory");
MODULE_LICENSE("GPL");
