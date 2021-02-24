// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * Trouble kernel module, which returns
 * fail or success based on random rand_number
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/random.h>


MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Module");
MODULE_LICENSE("Dual BSD/GPL");

#define SUCCESS_RATE_PERCENT 50 /* percentage of successfull module loads */

static uint rand_number;
module_param(rand_number, uint, 0);

static int __init da_mod_init(void)
{
	uint ret_value = 1;

	pr_info("Trouble module trying to start\n");

	get_random_bytes(&rand_number, 1);
	rand_number %= 100;

	pr_debug("rand_number = %d\n", rand_number);

	if (rand_number < SUCCESS_RATE_PERCENT) {
		pr_info("Unfortunately Trouble module started with no problem\n");
		ret_value = 0;
	}	else {
		pr_err("Trouble module failed successfully!\n");
	}

	return ret_value;
}

static void __exit da_mod_exit(void)
{
	pr_info("exit from Trouble module\n");
}

module_init(da_mod_init);
module_exit(da_mod_exit);
