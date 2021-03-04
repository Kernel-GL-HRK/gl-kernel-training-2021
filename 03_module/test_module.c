// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/printk.h>

MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training!");
MODULE_LICENSE("Dual BSD/GPL");

static int iparam;
module_param(iparam, int, 0);

static char *sparam = "";
module_param(sparam, charp, 0);

static int array[] = {0, 0, 0};
static int array_size = ARRAY_SIZE(array);
module_param_array(array, int, &array_size, 0644);

static int __init hello_init(void)
{
	pr_info("Hello, world!\n");
	pr_info("========== PASSED PARAMS ==========\n");
	pr_info("Integer passed parameter = %d\n", iparam);
	pr_info("String passed parameter = %s\n", sparam);

	if (array[0] < 0) {
		pr_err("Index cannot be negative in array parameter!\n");
		return -EINVAL;

	} else {
		int i;
		char buffer[32];

		sprintf(buffer, "array : ");
		for (i = 0; i < array_size; ++i)
			sprintf(buffer + strlen(buffer), "%d ", array[i]);

		pr_info("%s\n", buffer);
	}

	pr_info("===================================\n");
	return 0;
}

static void __exit hello_exit(void)
{
	// Do nothing here right now
}

module_init(hello_init);
module_exit(hello_exit);
