// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/time64.h>
#include <linux/time.h>
#include <linux/vmalloc.h>

MODULE_DESCRIPTION("Basic module demo: memory allocation");
MODULE_AUTHOR("AlexShlikhta oleksandr.shlikhta@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define NSEC_IN_SEC     1000000000LL
#define MAX_BUF_SIZE    64
#define NUM_OF_REPETITION	10

#define TABLE_HEAD "\
|------------------------------------------------------------------------------------------|\n\
|                                      %s TESTING                                     |\n\
|------------|--------------------------------------|--------------------------------------|\n\
|            |        Allocation time in nsec       |        Dealocation time in nsec      |\n\
|    SIZE    |------------|------------|------------|------------|------------|------------|\n\
|  in %s  |     MIN    |    AVER    |    MAX     |     MIN    |    AVER    |     MAX    |\n\
|------------|------------|------------|------------|------------|------------|------------|\n\
"

#define ROW_FORMAT "|        2^%2.d|  %10.lld|  %10.d|  %10.lld|  %10.lld|  %10.d|  %10.lld|\n"
#define ROW_FORMAT_GFP "|          %2.d|  %10.lld|  %10.d|  %10.lld|  %10.lld|  %10.d|  %10.lld|\n"

#define POW2(x)     (1LL << (x))

///////////////////////////////////////////////////////////////////////////////////////////////////

struct alloc_free_time {
	uint64_t alloc_time;
	uint64_t free_time;
	uint8_t *ptr;
};

enum mem_func {
	KMALLOC_F = 0,
	KZALLOC_F,
	VMALLOC_F,
	GET_FREE_PAGES_F
};

bool check_time(uint8_t pow_of_2, struct alloc_free_time *t, enum mem_func func)
{
	void *tmp_ptr;
	uint64_t t1, t2;
	uint64_t size = func == GET_FREE_PAGES_F ? pow_of_2 : POW2(pow_of_2);

	t1 = ktime_get_ns();

	switch (func) {

	case KMALLOC_F:
		tmp_ptr = kmalloc(size * sizeof(uint8_t), GFP_KERNEL);
	break;

	case KZALLOC_F:
		tmp_ptr = kzalloc(size * sizeof(uint8_t), GFP_KERNEL);
	break;

	case VMALLOC_F:
		tmp_ptr = vmalloc(size * sizeof(uint8_t));
	break;

	case GET_FREE_PAGES_F:
		tmp_ptr = (void *)__get_free_pages(GFP_KERNEL, size);
	break;

	default:
		tmp_ptr = NULL;
	break;
	}

	t2 = ktime_get_ns();

	t->ptr = tmp_ptr;

	if (tmp_ptr == NULL)
		return false;

	t->alloc_time = t2 - t1;

	t1 = ktime_get_ns();

	switch (func) {

	case KMALLOC_F:
	case KZALLOC_F:
		kfree(tmp_ptr);
	break;

	case VMALLOC_F:
		vfree(tmp_ptr);
	break;

	case GET_FREE_PAGES_F:
		free_pages((unsigned long)tmp_ptr, size);
	break;

	default:
		tmp_ptr = NULL;
	break;
	}

	t2 = ktime_get_ns();

	t->free_time = t2 - t1;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void test_and_print(char *text, enum mem_func func)
{
	struct alloc_free_time *t = kzalloc(sizeof(*t), GFP_KERNEL);
	struct alloc_free_time *t_aver = kzalloc(sizeof(*t_aver), GFP_KERNEL);
	struct alloc_free_time *t_max = kzalloc(sizeof(*t_aver), GFP_KERNEL);
	struct alloc_free_time *t_min = kzalloc(sizeof(*t_aver), GFP_KERNEL);
	bool flag;

	t_aver->alloc_time = 0;
	t_aver->free_time = 0;

	pr_info(TABLE_HEAD, text, func == GET_FREE_PAGES_F ? "pages" : "bytes");


	for (uint8_t i = 0; i < MAX_BUF_SIZE; i++) {

		uint8_t iter = NUM_OF_REPETITION;

		flag = check_time(i, t, func);

		if (flag == false) {
			pr_warn("Can't allocate:(\n");
			break;
		}

		t_max->alloc_time = t->alloc_time;
		t_max->free_time = t->free_time;
		t_min->alloc_time = t->alloc_time;
		t_min->free_time = t->free_time;

		iter--;

		do {

			t_aver->alloc_time += t->alloc_time;
			t_aver->free_time += t->free_time;

			check_time(i, t, func);

			t_max->alloc_time = max(t_max->alloc_time, t->alloc_time);
			t_max->free_time = max(t_max->free_time, t->free_time);
			t_min->alloc_time = max(t_min->alloc_time, t->alloc_time);
			t_min->free_time = max(t_min->free_time, t->free_time);


		} while (--iter);


		if (func == GET_FREE_PAGES_F)
			pr_info(ROW_FORMAT_GFP, i, t_min->alloc_time,
									(uint32_t)t_aver->alloc_time / MAX_BUF_SIZE,
									t_max->alloc_time, t_min->free_time,
									(uint32_t)t_aver->free_time / MAX_BUF_SIZE,
									t_max->free_time);
		else
			pr_info(ROW_FORMAT, i, t_min->alloc_time,
									(uint32_t)t_aver->alloc_time / MAX_BUF_SIZE,
									t_max->alloc_time, t_min->free_time,
									(uint32_t)t_aver->free_time / MAX_BUF_SIZE,
									t_max->free_time);
	}

	kfree(t);
	kfree(t_aver);
}

///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

int __init mod_init(void)
{
	pr_info("'task_05' module initialized\n");

	test_and_print("KMALLOC", KMALLOC_F);

	test_and_print("KZALLOC", KZALLOC_F);

	test_and_print(" GET_FP", GET_FREE_PAGES_F);

	test_and_print("VMALLOC", VMALLOC_F);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

void mod_cleanup(void)
{
	pr_info("'task_05' module released\n");
}

module_init(mod_init);
module_exit(mod_cleanup);
