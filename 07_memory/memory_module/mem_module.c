// SPDX-License-Identifier: GPL-3.0
/**/

#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/string.h>
#include <linux/module.h>
#include <linux/limits.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/log2.h>

struct STAT_ELEM {
long t_alloc;
long t_free;
unsigned long long num;
};

struct STAT_RES {
long t_alloc_max;
long t_alloc_min;
long t_alloc_sum;
long t_free_max;
long t_free_min;
long t_free_sum;
int cnt;
int order;
unsigned long long num;
};

#define KMALLOC_OP	0
#define KZALLOC_OP	1
#define VMALLOC_OP	2
#define GET_FPAGES	3

long delta_time(struct timespec64 start, struct timespec64 stop)
{
	struct timespec64 delta;

	delta.tv_sec = stop.tv_sec - start.tv_sec;
	delta.tv_nsec = stop.tv_nsec-start.tv_nsec;
	if (delta.tv_nsec < 0)
		delta.tv_nsec += 1000000000;
	return (long)(delta.tv_sec*1000000000 + delta.tv_nsec);
}

void print_stat(struct STAT_RES st_res)
{
	pr_info("======================================================\n");
	pr_info("ORDER: 2^%-2d BUF_SIZE: %-20lld\n",
		st_res.order, st_res.num);
	pr_info("ALLOC_TIME (nsec): ");
	pr_info("MAX = %-20ld AVG = %-20ld MIN = %-20ld\n",
		st_res.t_alloc_max,
		(st_res.t_alloc_sum / st_res.cnt),
		st_res.t_alloc_min);
	pr_info("FREE_TIME  (nsec): ");
	pr_info("MAX = %-20ld AVG = %-20ld MIN = %-20ld\n",
		st_res.t_free_max,
		(st_res.t_free_sum / st_res.cnt),
		st_res.t_free_min);
}

void update_statistic(struct STAT_ELEM st_item, struct STAT_RES *statistic)
{
	statistic->t_alloc_max = (st_item.t_alloc < statistic->t_alloc_max) ?
		statistic->t_alloc_max : st_item.t_alloc;
	statistic->t_alloc_min = (statistic->t_alloc_min) ?
		statistic->t_alloc_min : st_item.t_alloc;
	statistic->t_alloc_min = (st_item.t_alloc > statistic->t_alloc_min) ?
		statistic->t_alloc_min : st_item.t_alloc;
	statistic->t_alloc_sum += st_item.t_alloc;
	statistic->t_free_max = (st_item.t_free < statistic->t_free_max) ?
		statistic->t_free_max : st_item.t_free;
	statistic->t_free_min = (statistic->t_free_min) ?
		statistic->t_free_min : st_item.t_free;
	statistic->t_free_min = (st_item.t_free > statistic->t_free_min) ?
		statistic->t_free_min : st_item.t_free;
	statistic->t_free_sum += st_item.t_free;
	statistic->cnt += 1;
}
void set_elem(struct STAT_ELEM *st_item, struct STAT_RES statistic)
{
	memset(st_item, 0, sizeof(*st_item));
	st_item->num = statistic.num;
}

void alloc_mem(int operation, struct STAT_ELEM *st_el)
{
	void *ptr = NULL;
	static struct timespec64 start, stop;
	long page_order;
	unsigned long addr;

	switch (operation) {
	case KMALLOC_OP:
	case KZALLOC_OP:
		if ((st_el->num * sizeof(int)) >= SIZE_MAX)
			break;
		ktime_get_ts64(&start);
		ptr = (operation == KMALLOC_OP) ?
			kmalloc(((unsigned long)st_el->num * sizeof(int)),
				GFP_KERNEL) :
			kzalloc(((unsigned long)st_el->num * sizeof(int)),
				GFP_KERNEL);
		ktime_get_ts64(&stop);
		if (ptr == NULL)
			break;
		st_el->t_alloc = delta_time(start, stop);
		if (ptr != NULL) {
			ktime_get_ts64(&start);
			kfree(ptr);
			ktime_get_ts64(&stop);
		st_el->t_free = delta_time(start, stop);
		}
		break;
	case VMALLOC_OP:
		if ((unsigned long)(st_el->num * sizeof(int)) >= ULONG_MAX)
			break;
		if ((unsigned long)(st_el->num * sizeof(int)) >=
			VMALLOC_END-VMALLOC_START)
			break;
		ktime_get_ts64(&start);
		ptr = vmalloc((unsigned long)((st_el->num * sizeof(int))));
		ktime_get_ts64(&stop);
		if (ptr == NULL)
			break;
		st_el->t_alloc = delta_time(start, stop);
		if (ptr != NULL) {
			ktime_get_ts64(&start);
			vfree(ptr);
			ktime_get_ts64(&stop);
		st_el->t_free = delta_time(start, stop);
		}
		break;
	case GET_FPAGES:
		page_order = st_el->num / PAGE_SIZE * sizeof(int);
		page_order +=
			(st_el->num * sizeof(int) % PAGE_SIZE != 0) ? 1 : 0;
		page_order = order_base_2(page_order);
		if (page_order > MAX_ORDER-1)
			break;
		ktime_get_ts64(&start);
		addr = __get_free_pages(GFP_KERNEL, page_order);
		ktime_get_ts64(&stop);
		if (!addr)
			break;
		st_el->t_alloc = delta_time(start, stop);
		ktime_get_ts64(&start);
		free_pages(addr, page_order);
		ktime_get_ts64(&stop);
		st_el->t_free = delta_time(start, stop);
		break;
	default:
		break;
	}
}
unsigned long long l_pow(int base, int n)
{
	return (unsigned long long) (n == 0) ? 1 : base * l_pow(base, n - 1);
}

int check_mem(int operation)
{
	int order;
	int i;
	const int iorder = 64;
	const int istat = 5;
	struct STAT_ELEM stat_element;
	struct STAT_RES stat, stat_plus_1;


	for (order = 0; order <= iorder; order++) {
		memset(&stat, 0, sizeof(stat));
		memset(&stat_plus_1, 0, sizeof(stat_plus_1));
		stat.num = l_pow(2, order);
		stat_plus_1.num = stat.num + 1;
		stat.order = stat_plus_1.order = order;
		for (i = 0; i < istat; i++) {
			set_elem(&stat_element, stat);
			alloc_mem(operation, &stat_element);
			update_statistic(stat_element, &stat);
			set_elem(&stat_element, stat_plus_1);
			alloc_mem(operation, &stat_element);
			update_statistic(stat_element, &stat_plus_1);
		}
		print_stat(stat);
		print_stat(stat_plus_1);
	}
	return 0;
}

static int __init memtest_module_init(void)
{
	pr_debug("loading started\n");
	pr_info("==================KMALLOC TEST====================================\n");
	check_mem(KMALLOC_OP);
	pr_info("==================KZALLOC TEST====================================\n");
	check_mem(KZALLOC_OP);
	pr_info("==================VMALLOC TEST====================================\n");
	check_mem(VMALLOC_OP);
	pr_info("==============GET_FREE_PAGES TEST=================================\n");
	check_mem(GET_FPAGES);
	return 0;
}

static void __exit memtest_module_exit(void)
{
	pr_info("module exited\n");
}

module_init(memtest_module_init);
module_exit(memtest_module_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Memory allocation time test module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
