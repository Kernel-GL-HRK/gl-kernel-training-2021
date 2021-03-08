// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>


MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("Linux kernel memory");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

#define POW2(s) (1ULL << (s))

struct time_test_obj {
	char fun_name[20];
	u64 avg_allocate;
	u64 min_allocate;
	u64 max_allocate;
	u64 avg_deallocate;
	u64 min_deallocate;
	u64 max_deallocate;
	void *(*allocation)(u32 order, u32 delta, u64 *time);
	void (*deallocation)(void *ptr, u32 order, u32 delta, u64 *time);
};

u64 avarage(const u64 *arr, const u64 order)
{
	u64 accum = 0;
	u64 i = 0;

	for (i = 0; i < POW2(order); i++)
		accum += arr[i];

	return accum >> order;
}

u64 max_arr(const u64 *arr, const u64 order)
{
	u64 max = 0;
	u64 i = 0;

	for (i = 0; i < POW2(order); i++) {
		if (max < arr[i])
			max = arr[i];
	}
	return max;
}

u64 min_arr(const u64 *arr, const u64 order)
{
	u64 min = U64_MAX;
	u64 i = 0;

	for (i = 0; i < POW2(order); i++) {
		if (min > arr[i])
			min = arr[i];
	}
	return min;
}

void aldeal_get_time_stats(struct time_test_obj *test_obj, const u32 buf_order,
				const u32 delta, const u32 test_order)
{
	char *buf = NULL;
	u64 arr_alloc[POW2(test_order)];
	u64 arr_dealloc[POW2(test_order)];
	u64 time = 0;
	u32 i = 0;

	for (i = 0; i < POW2(test_order); i++) {
		buf = test_obj->allocation(buf_order, delta, &time);
		arr_alloc[i] = time;

		if (buf == NULL) {
			test_obj->avg_allocate = U64_MAX;
			test_obj->avg_deallocate = U64_MAX;
			return;
		}

		test_obj->deallocation(buf, buf_order, delta, &time);
		arr_dealloc[i] = time;
	}

	test_obj->avg_allocate = avarage(arr_alloc, test_order);
	test_obj->max_allocate = max_arr(arr_alloc, test_order);
	test_obj->min_allocate = min_arr(arr_alloc, test_order);

	test_obj->avg_deallocate = avarage(arr_dealloc, test_order);
	test_obj->max_deallocate = max_arr(arr_dealloc, test_order);
	test_obj->min_deallocate = min_arr(arr_dealloc, test_order);
}

u64 get_av_meas_time(u32 test_order)
{
	u64 t_begin = 0;
	u64 t_end =  0;
	u64 time_arr[POW2(test_order)];
	u32 i = 0;

	for (i = 0; i < POW2(test_order); i++) {
		t_begin = ktime_get_ns();
		t_end = ktime_get_ns();
		time_arr[i] = t_end - t_begin;
	}

	return avarage(time_arr, test_order);
}

void do_mem_range_test(struct time_test_obj *test_obj, const u32 max_range,
			const u32 test_order, const u64 meas_time,
			const u32 plus_delta)
{
	u64 no_del_all = 0;
	u64 no_del_all_max = 0;
	u64 no_del_all_min = 0;

	u64 no_del_deall = 0;
	u64 no_del_deall_max = 0;
	u64 no_del_deall_min = 0;

	u32 i = 0;

	pr_info("************TEST %s***********\n", test_obj->fun_name);

	pr_info("\n******2^X + %i*********\n", plus_delta);
	for (i = 0; i < max_range; i++) {

		aldeal_get_time_stats(test_obj, i, plus_delta, test_order);

		if (test_obj->avg_allocate == U64_MAX) {
			pr_info("Size: 2^%i + %i. NOT Allocated.\n",
							i, plus_delta);
			break;
		}

		no_del_all = test_obj->avg_allocate > meas_time ?
					test_obj->avg_allocate-meas_time:0;
		no_del_all_max = test_obj->max_allocate > meas_time ?
					test_obj->max_allocate-meas_time:0;
		no_del_all_min = test_obj->min_allocate > meas_time ?
					test_obj->min_allocate-meas_time:0;

		no_del_deall = test_obj->avg_deallocate > meas_time ?
					test_obj->avg_deallocate-meas_time:0;
		no_del_deall_max = test_obj->max_deallocate > meas_time ?
					test_obj->max_deallocate-meas_time:0;
		no_del_deall_min = test_obj->min_deallocate > meas_time ?
					test_obj->min_deallocate-meas_time:0;

		pr_info("Size: 2^%i + %i. Allocation TIME: %llu (%llu %llu) ns."
			" | Free Time: %llu (%llu %llu) ns\n", i, plus_delta,
			no_del_all, no_del_all_min, no_del_all_max,
			no_del_deall, no_del_deall_min, no_del_deall_max);
	}

	pr_info("************TEST END***********\n\n");
}



void *all_kmalloc(u32 order, u32 delta, u64 *time)
{
	u64 t_begin = 0;
	u64 t_end =  0;
	char *buf = NULL;

	t_begin = ktime_get_ns();
	buf = kmalloc(POW2(order) + delta, GFP_KERNEL);
	t_end = ktime_get_ns();

	*time = t_end - t_begin;

	return buf;
}

void *all_kzalloc(u32 order, u32 delta, u64 *time)
{
	u64 t_begin = 0;
	u64 t_end =  0;
	char *buf = NULL;

	t_begin = ktime_get_ns();
	buf = kzalloc(POW2(order) + delta, GFP_KERNEL);
	t_end = ktime_get_ns();

	*time = t_end - t_begin;

	return buf;
}

void *all_vmalloc(u32 order, u32 delta, u64 *time)
{
	u64 t_begin = 0;
	u64 t_end =  0;
	char *buf = NULL;

	t_begin = ktime_get_ns();
	buf = vmalloc(POW2(order) + delta);
	t_end = ktime_get_ns();

	*time = t_end - t_begin;

	return buf;
}

void *all_get_free_pages(u32 order, u32 delta, u64 *time)
{
	u64 t_begin = 0;
	u64 t_end =  0;
	char *buf = NULL;

	t_begin = ktime_get_ns();
	buf = alloc_pages_exact(POW2(order) + delta, GFP_KERNEL);
	t_end = ktime_get_ns();

	*time = t_end - t_begin;

	return buf;
}

void deall_kfree(void *ptr, u32 order, u32 delta, u64 *time)
{
	u64 t_begin = 0;
	u64 t_end =  0;

	t_begin = ktime_get_ns();
	kfree(ptr);
	t_end = ktime_get_ns();

	*time = t_end - t_begin;
}

void deall_vfree(void *ptr, u32 order, u32 delta, u64 *time)
{
	u64 t_begin = 0;
	u64 t_end =  0;

	t_begin = ktime_get_ns();
	vfree(ptr);
	t_end = ktime_get_ns();

	*time = t_end - t_begin;
}

void deall_free_pages(void *ptr, u32 order, u32 delta, u64 *time)
{
	u64 t_begin = 0;
	u64 t_end =  0;

	t_begin = ktime_get_ns();
	free_pages_exact(ptr, POW2(order)+delta);
	t_end = ktime_get_ns();

	*time = t_end - t_begin;
}

struct time_test_obj kmalloc_obj = {
	.fun_name = "KMALLOC",
	.allocation = all_kmalloc,
	.deallocation = deall_kfree
};

struct time_test_obj kzalloc_obj = {
	.fun_name = "KZALLOC",
	.allocation = all_kzalloc,
	.deallocation = deall_kfree
};

struct time_test_obj vmalloc_obj = {
	.fun_name = "VMALLOC",
	.allocation = all_vmalloc,
	.deallocation = deall_vfree
};

struct time_test_obj free_pages_obj = {
	.fun_name = "GET FREE PAGES",
	.allocation = all_get_free_pages,
	.deallocation = deall_free_pages
};


static int __init memory_init(void)
{
	u32 max_range = 64;
	u32 repeating_order = 5;
	u64 meas_time = 0;
	u32 i = 0;
	u32 j = 0;
	struct time_test_obj *test_obj_arr[] = {&kmalloc_obj,
						&kzalloc_obj,
						&vmalloc_obj,
						&free_pages_obj,
						NULL};

	meas_time = get_av_meas_time(6);
	pr_info("MEASURE TIME AVARAGE = %llu\n\n", meas_time);

	for (i = 0; test_obj_arr[i] != NULL; i++) {
		for (j = 0; j < 2; j += 1) {
			do_mem_range_test(test_obj_arr[i], max_range,
						repeating_order, meas_time, j);
		}
	}

	pr_info("module loaded\n");
	return 0;
}

static void __exit memory_exit(void)
{
	pr_info("module exited\n");
}

module_init(memory_init);
module_exit(memory_exit);
