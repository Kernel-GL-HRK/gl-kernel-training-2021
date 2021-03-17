// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/div64.h>

MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Module to test momery allocation/deallocation time");
MODULE_LICENSE("Dual BSD/GPL");

#define X_MAX_VALUE				22
#define	REPETITIONS	16


enum Allocation_Methods_t {
	kmalloc_method = 0,
	kzalloc_method,
	vmalloc_method,
	pages_method,
	allocation_methods_number
};

char m_names[allocation_methods_number][8] = {
		"kmalloc",
		"kzalloc",
		"vmalloc",
		"pages" };


void *allocate_testcase(u32 method, u64 size, u64 *time_x)
{
	char *buf = NULL;

	switch (method) {
	case kmalloc_method:
		*time_x = ktime_get_ns();
		buf = kmalloc(size, GFP_KERNEL);
		*time_x = ktime_get_ns() - *time_x;
		break;
	case kzalloc_method:
		*time_x = ktime_get_ns();
		buf = kzalloc(size, GFP_KERNEL);
		*time_x = ktime_get_ns() - *time_x;
		break;
	case vmalloc_method:
		*time_x = ktime_get_ns();
		buf = vmalloc(size);
		*time_x = ktime_get_ns() - *time_x;
		break;
	case pages_method:
		*time_x = ktime_get_ns();
		buf = alloc_pages_exact(size, GFP_KERNEL);
		*time_x = ktime_get_ns() - *time_x;
		break;
	default:
		time_x = 0;
		break;
	}

	return buf;
}



void deallocate_testcase(void *buf, u32 method, u64 size, u64 *time_x)
{
	switch (method) {
	case kmalloc_method:
		*time_x = ktime_get_ns();
		kfree(buf);
		*time_x = ktime_get_ns() - *time_x;
		break;
	case kzalloc_method:
		*time_x = ktime_get_ns();
		kfree(buf);
		*time_x = ktime_get_ns() - *time_x;
		break;
	case vmalloc_method:
		*time_x = ktime_get_ns();
		vfree(buf);
		*time_x = ktime_get_ns() - *time_x;
		break;
	case pages_method:
		*time_x = ktime_get_ns();
		free_pages_exact(buf, size);
		*time_x = ktime_get_ns() - *time_x;
		break;
	default:
		time_x = 0;
		break;
	}
}


u64 find_max(const u64 *arr, const u32 length)
{
	u32 i = 0;
	u64 max = arr[0];

	for (i = 1; i < length; i++) {
		if (max < arr[i])
			max = arr[i];
	}

	return max;
}


u64 find_average(const u64 *arr, const u32 length)
{
	u32 i = 0;
	u64 accum = 0;

	for (i = 0; i < length; i++)
		accum += arr[i];

	do_div(accum, length);  //first argument will become quotient

	return accum;
}


u64 find_min(const u64 *arr, const u32 length)
{
	u32 i = 0;
	u64 min = arr[0];

	for (i = 1; i < length; i++) {
		if (min > arr[i])
			min = arr[i];
	}

	return min;
}


static int __init memory_test_module_init(void)
{
	int ret_value = 0;
	char *buf = NULL;
	u32 method = kmalloc_method;
	u64 size = 0;
	u64 size_1 = 0;
	u32 x = 0;
	u32 j = 0;

	u64 alloc_times[REPETITIONS] = {0};
	u64 dealloc_times[REPETITIONS] = {0};


	pr_info("memory_test_module says: Lean back and "
		"wait for memory allocation test results!\n\n");

	for (method = kmalloc_method;
		 method < allocation_methods_number; method++) {

		pr_info("******************************** "
			"%s ********************************\n", m_names+method);
		pr_info("x   alloc (min\tavr\tmax)\t|"
			"dealloc (min\tavr\tmax), ns\n");

		for (x = 0; x < X_MAX_VALUE; x++) {
			size = 1ull << x;
			size_1 = size + 1u;

			for (j = 0; j < REPETITIONS; j++) {
				buf = allocate_testcase(method,
							size,
							&alloc_times[j]);
				deallocate_testcase(buf,
							method,
							size,
							&dealloc_times[j]);
			}
			pr_info("%llu\t%llu\t%llu\t%llu\t|"
				"\t%llu\t%llu\t%llu\n",
				size,
				find_min(alloc_times, REPETITIONS),
				find_average(alloc_times, REPETITIONS),
				find_max(alloc_times, REPETITIONS),
				find_min(dealloc_times, REPETITIONS),
				find_average(dealloc_times, REPETITIONS),
				find_max(dealloc_times, REPETITIONS));

			if (x != 0) {  //x = 0 -> size_1 = 2 (same as x = 1 -> size = 2)
				for (j = 0; j < REPETITIONS; j++) {
					buf = allocate_testcase(method,
						size_1,
						&alloc_times[j]);
					deallocate_testcase(buf,
						method,
						size_1,
						&dealloc_times[j]);
				}
				pr_info("%llu\t%llu\t%llu\t%llu\t|"
			"\t%llu\t%llu\t%llu\n",
			size_1,
			find_min(alloc_times, REPETITIONS),
			find_average(alloc_times, REPETITIONS),
			find_max(alloc_times, REPETITIONS),
			find_min(dealloc_times, REPETITIONS),
			find_average(dealloc_times, REPETITIONS),
			find_max(dealloc_times, REPETITIONS));
			}
		}
	}

	return ret_value;
}



static void __exit memory_test_module_exit(void)
{
	pr_info("memory_test_module says: Bye!\n");
}

module_init(memory_test_module_init);
module_exit(memory_test_module_exit);
