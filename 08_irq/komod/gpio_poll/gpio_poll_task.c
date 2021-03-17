// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 08_irq task gpio_poll
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kernel.h>

#ifndef KBUILD_MODNAME
#define KBUILD_MODNAME
#endif

static int __init gl_gpio_poll_init(void)
{
	pr_info("GL Kernel Training 2021\n");

	return 0;
}

static void __exit gl_gpio_poll_exit(void)
{
	pr_info("Exit from 08_irq module\n");
}

module_init(gl_gpio_poll_init);
module_exit(gl_gpio_poll_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 08_irq");
MODULE_LICENSE("GPL");
