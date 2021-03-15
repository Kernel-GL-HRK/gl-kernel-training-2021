// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 07_memory task
 *
 * Kernel module which tests allocation/freeing time for functions:
 *   - kmalloc
 *   - kzmalloc
 *   - vmalloc
 *   - get_free_pages
 * Module checks time of each allocation and freeing
 * Results are in text file table with columns:
 *   - buffer size
 *   - allocation time: max, min, average
 *   - freeing time:  min, av, max
 * Results can check using dmesg.
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include "memory_task.h"
#include "gl_time.h"

#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kernel.h>

#ifndef KBUILD_MODNAME
#define KBUILD_MODNAME
#endif

#define ALLOC_SIZE_BASE 2
#define ALLOC_SIZE_MAX_EXPONENT 64
#define ALLOC_COUNT ALLOC_SIZE_MAX_EXPONENT

struct gl_alloc_timestat {
	struct gl_time max;
	struct gl_time min;
	struct gl_time avg;
};

struct gl_alloc_mem_stat {
	size_t size;
	u32 error;
	struct gl_alloc_timestat alloc;
	struct gl_alloc_timestat free;
};

struct gl_stats {
	enum gl_allocator allocator_type;
	struct gl_alloc_mem_stat mem[ALLOC_COUNT];
};

size_t __gl_free_pages_size;

static u64 _ts;
static struct gl_time _tm;
struct gl_stats _stats[_ALLOCATOR_CNT];

static uint runs_num = 4;
module_param(runs_num, uint, 0);

static unsigned long gl_pow(unsigned int base, unsigned int exponent)
{
	unsigned long result = 1;

	if (exponent == 0)
		return result;

	while (exponent--)
		result *= base;

	return result;
}

static void gl_alloc_test(struct gl_stats *stats)
{
	void *p;
	int i;

	for (i = 0; i < ALLOC_SIZE_MAX_EXPONENT; ++i) {
		stats->mem[i].error = 0;
		stats->mem[i].size = gl_pow(ALLOC_SIZE_BASE, i);

		TIME_MEASURE(
			p = gl_alloc(stats->allocator_type, stats->mem[i].size);
		);

		stats->mem[i].alloc.avg = TIME_MEASURED(TIME_MEASURE_PRECISION);
		stats->mem[i].alloc.max = stats->mem[i].alloc.avg;
		stats->mem[i].alloc.min = stats->mem[i].alloc.avg;

		if (p == NULL) {
			stats->mem[i].error = i;
			break;
		}

		TIME_MEASURE(gl_dealloc(stats->allocator_type, p));
		stats->mem[i].free.avg = TIME_MEASURED(TIME_MEASURE_PRECISION);
		stats->mem[i].free.max = stats->mem[i].free.avg;
		stats->mem[i].free.min = stats->mem[i].free.avg;
	}
}

static void gl_calc_timestat(struct gl_stats **stats, size_t count)
{
	int i, j;
	struct gl_stats *_base = stats[0];

	for (j = 0; j < ALLOC_SIZE_MAX_EXPONENT; ++j) {
		if (_base->mem[j].error != 0)
			break;

		for (i = 1; i < count; ++i) {
			_base->mem[j].alloc.max = TMAX(_base->mem[j].alloc.max,
				stats[i]->mem[j].alloc.avg);
			_base->mem[j].alloc.min = TMIN(_base->mem[j].alloc.min,
				stats[i]->mem[j].alloc.avg);
			_base->mem[j].alloc.avg = TSUM(_base->mem[j].alloc.avg,
				stats[i]->mem[j].alloc.avg);

			_base->mem[j].free.max = TMAX(_base->mem[j].free.max,
				stats[i]->mem[j].free.avg);
			_base->mem[j].free.min = TMIN(_base->mem[j].free.min,
				stats[i]->mem[j].free.avg);
			_base->mem[j].free.avg = TSUM(_base->mem[j].free.avg,
				stats[i]->mem[j].free.avg);
		}

		_base->mem[j].alloc.avg = TDIV(_base->mem[j].alloc.avg, count);
		_base->mem[j].free.avg = TDIV(_base->mem[j].free.avg, count);
	}
}

static int gl_run_alloc_tests(struct gl_stats *stats, size_t count)
{
	int ret = 0, i;

	/* create array of gl_stats */
	struct gl_stats **_stats_arr = kmalloc(
		sizeof(void *) + count, GFP_KERNEL);
	if (_stats_arr == NULL) {
		ret = -1;
		goto exit;
	}

	/* init array of gl_stats */
	for (i = 0; i < count; ++i) {
		_stats_arr[i] = kmalloc(sizeof(struct gl_stats),
			GFP_KERNEL);
		if (_stats_arr[i] == NULL) {
			ret = -1;
			goto exit_free;
		}

		_stats_arr[i]->allocator_type = stats->allocator_type;
		gl_alloc_test(_stats_arr[i]);
	}

	gl_calc_timestat(_stats_arr, count);

	/* save results */
	memcpy((void *)stats, (void *)_stats_arr[0],
		sizeof(struct gl_stats));

exit_free:
	for (i = 0; i < count; ++i)
		if (_stats_arr[i] != NULL)
			kfree(_stats_arr[i]);
	if (_stats_arr != NULL)
		kfree(_stats_arr);
exit:
	return ret;
}

static int gl_run_all_tests(void)
{
	int ret, i;

	for (i = 0; i < (int)_ALLOCATOR_CNT; ++i) {
		_stats[i].allocator_type = (enum gl_allocator)i;
		ret = gl_run_alloc_tests(&_stats[i], runs_num);
		if (ret != 0)
			return ret;
	}

	return ret;
}

static void gl_display_results(void)
{
	int i, j;
	char *alloc_name;

	for (i = 0; i < (int)_ALLOCATOR_CNT; ++i) {
		alloc_name = gl_get_allocator_name(_stats[i].allocator_type);
		pr_info("Test %s, number of runs: %u\n", alloc_name, runs_num);
		pr_info("\tsize\t%*s%s\t%*s%s\t%*s%s\t%*s%s\t%*s%s\t%*s%s",
			2, "", "alloc, max", 2, "", "alloc, min",
			2, "", "alloc, avg", 3, "", "free, max",
			3, "", "free, min", 3, "", "free, avg");

		for (j = 0; j < ALLOC_SIZE_MAX_EXPONENT; j++) {
			if (_stats[i].mem[j].error != 0) {
				pr_info("Test %s fails on 2^%02d (%lu bytes)\n",
					alloc_name, j,
					gl_pow(ALLOC_SIZE_BASE, j));
				break;
			}

			pr_info("\t2^%02d"
				TIME_PR_TEMPLATE TIME_PR_TEMPLATE
				TIME_PR_TEMPLATE TIME_PR_TEMPLATE
				TIME_PR_TEMPLATE TIME_PR_TEMPLATE
				"\n",
				j,
				_stats[i].mem[j].alloc.max.tm_sec,
				_stats[i].mem[j].alloc.max.tm_nsec,
				_stats[i].mem[j].alloc.min.tm_sec,
				_stats[i].mem[j].alloc.min.tm_nsec,
				_stats[i].mem[j].alloc.avg.tm_sec,
				_stats[i].mem[j].alloc.avg.tm_nsec,
				_stats[i].mem[j].free.max.tm_sec,
				_stats[i].mem[j].free.max.tm_nsec,
				_stats[i].mem[j].free.min.tm_sec,
				_stats[i].mem[j].free.min.tm_nsec,
				_stats[i].mem[j].free.avg.tm_sec,
				_stats[i].mem[j].free.avg.tm_nsec);
		}
		pr_info("\n");
	}
}

static int __init gl_memory_init(void)
{

	pr_info("GL Kernel Training 2021\n");

	if (gl_run_all_tests() != 0)
		return -1;

	gl_display_results();

	return 0;
}

static void __exit gl_memory_exit(void)
{
	pr_info("Exit from 07_memory module\n");
}

module_init(gl_memory_init);
module_exit(gl_memory_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 07_memory");
MODULE_LICENSE("GPL");
