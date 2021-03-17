// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

#include <linux/vmalloc.h>
#include <linux/types.h>
#include <linux/slab.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("List data module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

#define MEMTEST_ATEMPTS_POWER (4)
#define MEMTEST_ATEMPTS (1<<MEMTEST_ATEMPTS_POWER)

#define MEMTEST_SIZE(x) (1ll<<(x))
#define MEMTEST_LINMIT	(64)

struct mem_ops {
	char head_str[16];
	void* (*space_alloc)(size_t size);
	void (*space_free)(const void *ptr, size_t size);
};

struct time_stats {
	u64 min_ns;
	u64 max_ns;
	u64 average_ns;
};

static struct time_stats alloc_stats = {U64_MAX, 0, 0};
static struct time_stats free_stats = {U64_MAX, 0, 0};
static u64 ktime_delta;

static void update_stats(struct time_stats *stats, u64 time_ns)
{
	time_ns = (time_ns > ktime_delta) ? (time_ns - ktime_delta) : 0;

	stats->average_ns += time_ns;

	if (time_ns > stats->max_ns)
		stats->max_ns = time_ns;
	if (time_ns < stats->min_ns)
		stats->min_ns = time_ns;
}

static void process_stats(struct time_stats *stats, const char *msg)
{
	stats->average_ns >>= MEMTEST_ATEMPTS_POWER;

	pr_info("%s: min [%llu ns], max [%llu ns], average [%llu ns]\n",
		msg, stats->min_ns, stats->max_ns, stats->average_ns);

	stats->average_ns = 0;
	stats->min_ns = U64_MAX;
	stats->max_ns = 0;
}

static int memtest(u8 stage, struct mem_ops *test_func)
{
	u8 indx = 0;
	u64 time_delta = 0;
	char *buffer = NULL;

	if (test_func == NULL) {
		pr_err("%s, mem_ops pointer is NULL",
		       __func__);
		return -EINVAL;
	}

	// pull testes functions to cache
	buffer = test_func->space_alloc(1);
	if (buffer == NULL) {
		pr_err("%s, failed to cashe function %s",
		       __func__, test_func->head_str);
		return -ENOMEM;
	}
	test_func->space_free(buffer, 1);

	pr_info("Runing test with [%s] function at size: [2^%d] bytes",
		test_func->head_str,
		stage);

	while (indx < MEMTEST_ATEMPTS) {
		indx++;

		time_delta = ktime_get_ns();
		buffer = test_func->space_alloc(MEMTEST_SIZE(stage));
		time_delta = ktime_get_ns() - time_delta;

		if (unlikely(buffer == NULL)) {
			pr_err("%s, alloc failed at size: 2^%d bytes, atempt: %d",
			       __func__, stage, indx);
			return -ENOMEM;
		}

		update_stats(&alloc_stats, time_delta);

		time_delta = ktime_get_ns();
		test_func->space_free(buffer, MEMTEST_SIZE(stage));
		time_delta = ktime_get_ns() - time_delta;
		/* emd time meas */

		update_stats(&free_stats, time_delta);
	}
	process_stats(&alloc_stats, "[STATS] alloc");
	process_stats(&free_stats, "[STATS] free");

	return 0;
}


static void *my_kmalloc(size_t size)
{
	return kmalloc(size, GFP_KERNEL);
}
static void my_kfree(const void *ptr, size_t size)
{
	kfree(ptr);
}

static void *my_kzalloc(size_t size)
{
	return kzalloc(size, GFP_KERNEL);
}
static void my_kzfree(const void *ptr, size_t size)
{
	kzfree(ptr);
}

static void *my_vmalloc(size_t size)
{
	return vmalloc(size);
}
static void my_vfree(const void *ptr, size_t size)
{
	vfree(ptr);
}

static void *my_get_free_pages(size_t size)
{
	unsigned int order = get_order(size);

	return (void *)__get_free_pages(GFP_KERNEL, order);
}
static void my_free_pages(const void *ptr, size_t size)
{
	unsigned int order = get_order(size);

	free_pages((unsigned long)ptr, order);
}

static void calibrate_ktime(void)
{
	u8 iteration = MEMTEST_ATEMPTS;
	u64 time_stamp;

	while (iteration--) {
		time_stamp = ktime_get_ns();
		ktime_delta += ktime_get_ns() - time_stamp;
	}

	ktime_delta >>= MEMTEST_ATEMPTS_POWER;

	pr_info("%s, ktime dump call delta: %llu\n",
		__func__, ktime_delta);
}

static int memtest_module_init(void)
{
	int res = 0;
	u8 stage = 0;
	u8 funk_indx;
	struct mem_ops memtest_func[] = {
		{"KMALLOC", &my_kmalloc, &my_kfree},
		{"KZMALLOC", &my_kzalloc, &my_kzfree},
		{"VMALLOC", &my_vmalloc, &my_vfree},
		{"GET_FREE_PAGES", &my_get_free_pages, &my_free_pages}
	};

	pr_info("%s: module starting\n",  __func__);

	calibrate_ktime();

	while (stage < MEMTEST_LINMIT) {
		for (funk_indx = 0;
		     funk_indx < ARRAY_SIZE(memtest_func);
		     funk_indx++) {
			res = memtest(stage, &memtest_func[funk_indx]);
			if (res < 0)
				return res;
		}
		stage++;
	}

	return res;
}

static void memtest_module_exit(void)
{
	pr_info("%s: module exit\n",  __func__);
}

module_init(memtest_module_init);
module_exit(memtest_module_exit);
