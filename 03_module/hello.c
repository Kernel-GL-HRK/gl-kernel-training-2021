// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 03_module task
 *
 * Simplest kernel loadable module which outputs "Hello, world!" message
 * depending on the passed parameter
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>

#define PARAM_ZERO 0 /* mod warning parameter zero value */
#define PARAM_LWBND 5 /* mod warning parameter value lower bound */
#define PARAM_UPBND 10 /* mod warning parameter value upper bound */

static uint print_cnt = 1; /* the default parameter is 1 */
module_param(print_cnt, uint, 0);

static int __init gl_hello_init(void)
{
	pr_info("GL Kernel Training 2021\n");
	pr_debug("print_cnt is %d\n", print_cnt); /* some debug print */

	if (print_cnt > PARAM_UPBND) {
		pr_err("print_cnt cannot be bigger than %d!\n", PARAM_UPBND);
		return -EINVAL;
	}

	if (print_cnt >= PARAM_LWBND) {
		pr_warn("print_cnt is %d, there can be a lot of output!\n",
			print_cnt);
	}

	if (print_cnt != PARAM_ZERO) {
		int i;

		for (i = 0; i < print_cnt; ++i)
			pr_info("Hello, world!\n");
	} else {
		pr_warn("print_cnt is 0, you won't see any output!\n");
	}

	return 0;
}

static void __exit gl_hello_exit(void)
{
	pr_info("exit from hello module\n");
}

module_init(gl_hello_init);
module_exit(gl_hello_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 03_module");
MODULE_LICENSE("GPL");
