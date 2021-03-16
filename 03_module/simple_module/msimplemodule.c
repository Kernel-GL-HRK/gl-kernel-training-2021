// SPDX-License-Identifier: GPL-3.0
#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/init.h>	// Macros used to mark up functions  __init __exit
#include <linux/module.h>	// Core header for loading LKMs into the kernel
#include <linux/kernel.h>	// Contains types, macros, functions for the kernel
#include <linux/moduleparam.h>

static int iparam = -1;
module_param(iparam, int, 0);

static int __init hello_init(void)
{
	pr_debug("loading started\n");
	pr_info("iparam = %d\n", iparam);
	return iparam;
}

static void __exit hello_exit(void)
{
	pr_debug("module exits\n");
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Simple Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
