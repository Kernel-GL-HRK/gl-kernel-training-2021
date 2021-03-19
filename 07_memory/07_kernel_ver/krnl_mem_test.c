// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/vmalloc.h>
#include <asm/div64.h>
#include <linux/printk.h>

MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Memory managenent in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

#define MAX_SIZE	64
#define LOOP_SIZE	10

#define POW2(x) (1ULL << (x))
#define MIN(x, y) ((x < y)?x:y)
#define MAX(x, y) ((x > y)?x:y)

char *mem_func_name[] = {"kmalloc", "kzalloc", "vmalloc", "GFP"};

struct mem_live_time {
	uint64_t alloc_time;
	uint64_t free_time;
};

enum mem_func_name {
	KMALLOC = 0,
	KZALLOC,
	VMALLOC,
	GET_FREE_PAGES,
	NUM_FN
};

enum test_case {
	TWO_POW_X = 0,
	TWO_POW_X_PLUS_ONE,
	NUM_TC
};


int get_time(uint8_t order, enum test_case tc_num,
		struct mem_live_time *t_mem_live, enum mem_func_name func_name)
{
	uint64_t t_start = 0;
	uint64_t t_stop = 0;
	char *mem_ptr = NULL;
	uint64_t size;

	size = (tc_num == TWO_POW_X)?POW2(order):(POW2(order) + 1);

	t_start = ktime_get_ns();

	switch (func_name) {

	case KMALLOC:
		mem_ptr = kmalloc(size * sizeof(*mem_ptr), GFP_KERNEL);
		break;

	case KZALLOC:
		mem_ptr = kzalloc(size * sizeof(*mem_ptr), GFP_KERNEL);
		break;

	case VMALLOC:
		mem_ptr = vmalloc(size * sizeof(*mem_ptr));
		break;

	case GET_FREE_PAGES:
		if (order > 0)
			mem_ptr = (void *)__get_free_pages(GFP_KERNEL, order);
		break;

	default:
		mem_ptr = NULL;
		break;
	}

	t_stop = ktime_get_ns();

	if (mem_ptr == NULL)
		return 1;

	t_mem_live->alloc_time = t_stop - t_start;

	t_start = ktime_get_ns();

	switch (func_name) {

	case KMALLOC:
	case KZALLOC:
		kfree(mem_ptr);
		break;

	case VMALLOC:
		vfree(mem_ptr);
		break;

	case GET_FREE_PAGES:
		if (order > 0)
			free_pages((unsigned long)mem_ptr, order);
		break;

	default:
		mem_ptr = NULL;
		break;
	}

	t_stop = ktime_get_ns();

	t_mem_live->free_time = t_stop - t_start;

	return 0;
}

void run_test_mem(enum test_case tc_num, enum mem_func_name func_name)
{
	struct mem_live_time t_current = {0};
	uint64_t t_min_alloc = U64_MAX;
	uint64_t t_max_alloc = 0;
	uint64_t t_min_free = U64_MAX;
	uint64_t t_max_free = 0;
	uint64_t t_sum_alloc = 0;
	uint64_t t_sum_free = 0;

	int i = 0;
	int j = 0;

	pr_info("====== Run memory test allocation! ============================\n");
	pr_info("|\n");
	pr_info("| function call: %s\n", mem_func_name[func_name]);
	pr_info("| test case:     %s\n", (tc_num)?"2^x + 1":"2^x");
	pr_info("|\n");
	pr_info("===============================================================\n");


	for (i = 0; i < MAX_SIZE; i++) {

		pr_info("| x: %d", i);

		t_min_alloc = U64_MAX;
		t_max_alloc = 0;
		t_min_free = U64_MAX;
		t_max_free = 0;
		t_sum_alloc = 0;
		t_sum_free = 0;

		for (j = 0; j < LOOP_SIZE; j++) {

			if (get_time(i, tc_num, &t_current, func_name)) {
				pr_info("| ### Memory cannot be allocated!\n");
				return;
			}

			t_min_alloc = MIN(t_current.alloc_time, t_min_alloc);
			t_max_alloc = MAX(t_current.alloc_time, t_max_alloc);
			t_min_free = MIN(t_current.free_time, t_min_free);
			t_max_free = MAX(t_current.free_time, t_max_free);
			t_sum_alloc += t_current.alloc_time;
			t_sum_free += t_current.free_time;

		}

		pr_info("| alloc time:  min: %llu(ns)"
				" average: %llu(ns)"
				" max: %llu(ns)\t"
				"| free time:  min: %llu(ns)"
				" average: %llu(ns)"
				" max: %llu(ns)"
				, t_min_alloc, (uint32_t)t_sum_alloc/LOOP_SIZE, t_max_alloc,
				t_min_free, (uint32_t)t_sum_free/LOOP_SIZE, t_max_free);

	}

}

static int __init mod_init(void)
{
	int i;
	int j;

	pr_info("=== krnl_mem_test module loaded! ===\n");

	for (i = 0; i < NUM_TC; i++) {
		for (j = 0; j < NUM_FN; j++)
			run_test_mem(i, j);
	}

	return 0;
}

static void __exit mod_exit(void)
{
	pr_info("--- krnl_mem_test module unloaded! ---\n");
}

module_init(mod_init);
module_exit(mod_exit);
