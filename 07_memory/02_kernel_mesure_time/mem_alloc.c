// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include <media/rc-core.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define MODULE_NAME "mem_alloc"
#define MEM_MAX_PWR 64
#define AMOUNT_TESTS 10
#define NSEC_IN_SEC 1000000000
#define TABLE_HEADER "|----------------------------------------------------------------------------------------|\n|                                    Testing %s()                                    |\n|----------|--------------------------------------|--------------------------------------|\n|          |         Alloc time sec               |          Free time sec               |\n| Buf size |------------|------------|------------|------------|------------|------------|\n|          |    Min     |   Average  |    Max     |    Min     |   Average  |    Max     |\n|----------|------------|------------|------------|------------|------------|------------|\n"
#define TABLE_DELIMETER "|----------|------------|------------|------------|------------|------------|------------|\n"
#define TABLE_ROW "|%10lu|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|\n"



#define SET_MIN_SPEC(min, spec) do {\
		if ((!min.tv_sec && !min.tv_nsec) ||\
				(min.tv_sec > spec.tv_sec)) {\
			memcpy(&min, &spec, sizeof(spec));\
			break;\
		} \
		if (min.tv_nsec > spec.tv_nsec)\
			min.tv_nsec = spec.tv_nsec;\
	} while (0)

#define SET_MAX_SPEC(max, spec) do { if ((!max.tv_sec && !max.tv_nsec) ||\
				(max.tv_sec < spec.tv_sec)) {\
			memcpy(&max, &spec, sizeof(spec));\
			break;\
		} \
		if (max.tv_nsec < spec.tv_nsec)\
			max.tv_nsec = spec.tv_nsec;\
	} while (0)

#define ADD_TO_AVRG_SPEC(avrg, spec) (avrg.tv_nsec += spec.tv_sec *\
		NSEC_IN_SEC + spec.tv_nsec)

enum {
	ALLOC_MIN_SPEC = 0,
	ALLOC_AVRG_SPEC,
	ALLOC_MAX_SPEC,
	FREE_MIN_SPEC,
	FREE_AVRG_SPEC,
	FREE_MAX_SPEC,
	AMOUNT_SPEC
};

enum {
	KMALLOC_TYPE = 0,
	KZALLOC_TYPE,
	VMALLOC_TYPE,
	GET_FREE_PAGES_TYPE
};

static void *memory_alloc_check_time(unsigned long size, int type,
		struct timespec *ret_spec)
{
	void *ptr;
	long nsec;
	ktime_t timestamp;
	struct timespec spec, cmp_spec;

	timestamp = ktime_get();
	spec = ktime_to_timespec(timestamp);

	switch (type) {
	case KMALLOC_TYPE:
		ptr = kmalloc(size, GFP_KERNEL);
		break;
	case KZALLOC_TYPE:
		ptr = kzalloc(size, GFP_KERNEL);
		break;
	case VMALLOC_TYPE:
		ptr = vmalloc(size);
		break;
	case GET_FREE_PAGES_TYPE:
		ptr = (void *)__get_free_pages(GFP_KERNEL, size);
		break;
	default:
		ptr = NULL;
		break;
	}

	if (!ptr) {
		pr_err("%s error", __func__);
		return NULL;
	}

	timestamp = ktime_get();
	cmp_spec = ktime_to_timespec(timestamp);

	nsec = cmp_spec.tv_nsec - spec.tv_nsec;
	if (nsec < 0) {
		nsec += NSEC_IN_SEC;
		cmp_spec.tv_sec -= 1;
	}

	ret_spec->tv_sec = cmp_spec.tv_sec - spec.tv_sec;
	ret_spec->tv_nsec = nsec;

	return ptr;
}

static void memory_free_check_time(void *ptr, int type,
		struct timespec *ret_spec, unsigned long size)
{
	long nsec;
	ktime_t timestamp;
	struct timespec spec, cmp_spec;

	timestamp = ktime_get();
	spec = ktime_to_timespec(timestamp);

	switch (type) {
	case KMALLOC_TYPE:
		kfree(ptr);
		break;
	case KZALLOC_TYPE:
		kzfree(ptr);
		break;
	case VMALLOC_TYPE:
		vfree(ptr);
		break;
	case GET_FREE_PAGES_TYPE:
		free_pages((unsigned long)ptr, size);
		/* Stack memory no need to be free */
		return;
	default:
		break;
	}

	timestamp = ktime_get();
	cmp_spec = ktime_to_timespec(timestamp);

	nsec = cmp_spec.tv_nsec - spec.tv_nsec;
	if (nsec < 0) {
		nsec += NSEC_IN_SEC;
		cmp_spec.tv_sec -= 1;
	}

	ret_spec->tv_sec = cmp_spec.tv_sec - spec.tv_sec;
	ret_spec->tv_nsec = nsec;
}

static int memory_check_size(int type, unsigned long size)
{
	int i;
	void *ptr;
	struct timespec spec, arr_spec[AMOUNT_SPEC] = {0};

	for (i = 0; i < AMOUNT_TESTS; i++) {
		/* Allocate memory and save allocation time */
		ptr = memory_alloc_check_time(size, type, &spec);
		if (!ptr)
			return -1;

		ADD_TO_AVRG_SPEC(arr_spec[ALLOC_AVRG_SPEC], spec);
		SET_MIN_SPEC(arr_spec[ALLOC_MIN_SPEC], spec);
		SET_MAX_SPEC(arr_spec[ALLOC_MAX_SPEC], spec);

		/* Free memory and save this time */
		memory_free_check_time(ptr, type, &spec, size);
		ADD_TO_AVRG_SPEC(arr_spec[FREE_AVRG_SPEC], spec);
		SET_MIN_SPEC(arr_spec[FREE_MIN_SPEC], spec);
		SET_MAX_SPEC(arr_spec[FREE_MAX_SPEC], spec);
	}

	/* Time output into the table */
	pr_info(TABLE_ROW, size, arr_spec[ALLOC_MIN_SPEC].tv_sec,
		arr_spec[ALLOC_MIN_SPEC].tv_nsec,
		arr_spec[ALLOC_AVRG_SPEC].tv_nsec
		/ AMOUNT_TESTS / NSEC_IN_SEC,
		arr_spec[ALLOC_AVRG_SPEC].tv_nsec /
		AMOUNT_TESTS % NSEC_IN_SEC,
		arr_spec[ALLOC_MAX_SPEC].tv_sec,
		arr_spec[ALLOC_MAX_SPEC].tv_nsec,
		arr_spec[FREE_MIN_SPEC].tv_sec,
		arr_spec[FREE_MIN_SPEC].tv_nsec,
		arr_spec[FREE_AVRG_SPEC].tv_nsec
		/ AMOUNT_TESTS / NSEC_IN_SEC,
		arr_spec[FREE_AVRG_SPEC].tv_nsec /
		AMOUNT_TESTS % NSEC_IN_SEC,
		arr_spec[FREE_MAX_SPEC].tv_sec,
		arr_spec[FREE_MAX_SPEC].tv_nsec);

	return 0;
}

static void memory_test(int type)
{
	unsigned int i;

	for (i = 0; i < MEM_MAX_PWR; i++) {
		if (memory_check_size(type, (unsigned long)(1 << i)) == -1)
			break;

		if (i && memory_check_size(type, (unsigned long)((1 << i) + 1))
			== -1)
			break;
	}
}

static int __init mem_alloc_init(void)
{
	pr_info(TABLE_HEADER, "kmalloc");
	memory_test(KMALLOC_TYPE);
	pr_info(TABLE_HEADER, "kzalloc");
	memory_test(KZALLOC_TYPE);
	pr_info(TABLE_HEADER, "vmalloc");
	memory_test(VMALLOC_TYPE);
	pr_info(TABLE_HEADER, "get_free_pages");
	memory_test(GET_FREE_PAGES_TYPE);
	pr_info(MODULE_NAME ": init success\n");

	return 0;
}

static void __exit mem_alloc_exit(void)
{
	pr_info(MODULE_NAME ": module release!\n");
}

module_init(mem_alloc_init);
module_exit(mem_alloc_exit);
