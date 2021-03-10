// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>

MODULE_AUTHOR("Ivan Kryvosheia");
MODULE_DESCRIPTION("Test module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.01");

static int bbb_arch_size;
module_param(bbb_arch_size, int, 0);
MODULE_PARM_DESC(bbb_arch_size, "BBB architecture size");

#define BBB_ARCH 32

static int __init lkm_example_init(void)
{
	int rc = 0;

	pr_info("Hello, World!\n");

	if (bbb_arch_size ==  BBB_ARCH) {
		pr_info("Yes! BBB architecture size is: %d\n", bbb_arch_size);
		rc = 0;
	} else {
		pr_warn("Not really. BBB architecture size is: %d\n", BBB_ARCH);
		rc = -EINVAL;
	}
	return rc;
}

static void __exit lkm_example_exit(void)
{
	pr_info("Goodbye, World!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
