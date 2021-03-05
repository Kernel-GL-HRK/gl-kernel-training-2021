// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 05_debug task
 *
 * Kernel loadable module which creates a kobject/proc with representation
 * in the sysfs/procfs virtual filesystem, kobject/proc contains sysfs/proc
 * attribute with name 'lowercase'/'uppercase' which implements
 * character-to-lowercase and character-to-uppercase converter.
 * Also module collects a statistics about its use: total module calls,
 * total characters processed and characters converted.
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>
#include "debug_procfs.h"

static int __init gl_debug_init(void)
{
	int retval;

	pr_info("GL Kernel Training 2021\n");

	retval = gl_procfs_init();
	if (retval)
		return retval;

	return 0;
}

static void __exit gl_debug_exit(void)
{
	gl_procfs_deinit();
	pr_info("Exit from 05_debug module\n");
}

module_init(gl_debug_init);
module_exit(gl_debug_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 05_debug");
MODULE_LICENSE("GPL");
