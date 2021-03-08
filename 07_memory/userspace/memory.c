#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>

#define NSEC_PER_SEC	1000000000ULL

#define POW2(s) (1ULL << (s))

struct time_test_obj {
	char fun_name[10];
	unsigned long long avg_allocate;
	unsigned long long avg_deallocate;
	void *(*allocation)(unsigned long long, unsigned long long, unsigned long long*);
	void (*deallocation)(void *, unsigned long long*);
};

unsigned long long diff_tm_ns(const struct timespec *t_begin, 
			const struct timespec *t_end)
{
	return  ((t_end->tv_sec*NSEC_PER_SEC) + t_end->tv_nsec) - 
		((t_begin->tv_sec*NSEC_PER_SEC) + t_begin->tv_nsec);
}	

unsigned long long avarage(const unsigned long long *arr, const int size)
{	
	unsigned long long accum = 0;
	for (int i = 0; i < size; i++) {
		accum += arr[i];
	}
	return accum/(unsigned long long)size;	
}

void aldeal_get_time_stats(struct time_test_obj *test_obj, 
			const unsigned long long buf_size, const int test_count)
{
	char *buf = NULL;
	unsigned long long arr_alloc[test_count];
	unsigned long long arr_dealloc[test_count];
	unsigned long long time = 0;

	for (int i = 0; i < test_count; i++) {
		buf = test_obj->allocation(buf_size, sizeof(char), &time);
		arr_alloc[i] = time;

		if (buf == NULL) {
			test_obj->avg_allocate = ULLONG_MAX;
			test_obj->avg_deallocate = ULLONG_MAX;
			return;
		}

		test_obj->deallocation(buf, &time);
		arr_dealloc[i] = time;
	}
	
	test_obj->avg_allocate = avarage(arr_alloc, test_count);
	test_obj->avg_deallocate = avarage(arr_dealloc, test_count);
}

unsigned long long get_av_meas_time(const int test_count)
{
	struct timespec t_begin = {0};
	struct timespec t_end = {0};

	unsigned long long time_arr[test_count];

	for (int i = 0; i < test_count; i++)
	{
		clock_gettime(CLOCK_MONOTONIC, &t_begin);
		clock_gettime(CLOCK_MONOTONIC, &t_end);
		time_arr[i] = diff_tm_ns(&t_begin, &t_end);
	}
	return avarage(time_arr, test_count);
}

void do_mem_range_test(struct time_test_obj *test_obj, const int max_range, 
			const int test_count, unsigned long long meas_time, 
			int plus_delta) 
{
	unsigned long long without_delay_alloc = 0;
	unsigned long long without_delay_deall = 0;

	printf("************TEST %s***********\n", test_obj->fun_name);

	printf("\n******2^X + %i*********\n", plus_delta);
	for (int i = 0; i < max_range; i++) {
				
		unsigned long long buf_size = POW2(i) + (unsigned long long)plus_delta;
		
		aldeal_get_time_stats(test_obj, buf_size, test_count);

		if (test_obj->avg_allocate == ULLONG_MAX) {
			printf("Size: 2^%i + %i. NOT Allocated.\n", 
							i, plus_delta);
			break;	
		} 

		without_delay_alloc = test_obj->avg_allocate > meas_time?
					test_obj->avg_allocate - meas_time:0;
		without_delay_deall = test_obj->avg_deallocate > meas_time?
					test_obj->avg_deallocate - meas_time:0;

		printf("Size: 2^%i + %i. Allocation TIME: %llu (%llu) ns." 
				" | Free Time: %llu (%llu) ns\n", i, plus_delta, 
				test_obj->avg_allocate, without_delay_alloc, 
				test_obj->avg_deallocate, without_delay_deall);
	}
	
	printf("************TEST END***********\n\n");
}



void *all_malloc(unsigned long long count, unsigned long long size_of, unsigned long long *time)
{
	struct timespec t_begin = {0};
	struct timespec t_end = {0};
	char *buf = NULL;

	clock_gettime(CLOCK_MONOTONIC, &t_begin);
	buf = malloc(count * size_of);
	clock_gettime(CLOCK_MONOTONIC, &t_end);

	*time = diff_tm_ns(&t_begin, &t_end);

	return buf;
}

void *all_calloc(unsigned long long count, unsigned long long size_of, unsigned long long *time)
{
	struct timespec t_begin = {0};
	struct timespec t_end = {0};
	char *buf = NULL;

	clock_gettime(CLOCK_MONOTONIC, &t_begin);
	buf =calloc(count, size_of);
	clock_gettime(CLOCK_MONOTONIC, &t_end);

	*time = diff_tm_ns(&t_begin, &t_end);

	return buf;
}

void *all_alloca(unsigned long long count, unsigned long long size_of, unsigned long long *time)
{	
	struct timespec t_begin = {0};
	struct timespec t_end = {0};
	char *buf = NULL;
	struct rlimit limit = {0};
	int cur_stack_usage = 10000; //approx magic number
	
	getrlimit(RLIMIT_STACK, &limit);

	if ((count * size_of) > (limit.rlim_cur - cur_stack_usage)) {
		*time = 0;
		return NULL;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &t_begin);
	buf = alloca(count * size_of);
	clock_gettime(CLOCK_MONOTONIC, &t_end);

	*time = diff_tm_ns(&t_begin, &t_end);

	return malloc(1); // we need return rubbish for pass NULL check
}

void deall_free(void *ptr, unsigned long long *time)
{
	struct timespec t_begin = {0};
	struct timespec t_end = {0};

	clock_gettime(CLOCK_MONOTONIC, &t_begin);
	free(ptr);
	clock_gettime(CLOCK_MONOTONIC, &t_end);

	*time = diff_tm_ns(&t_begin, &t_end);
}

void deall_none(void *ptr, unsigned long long *time) 
{	
	*time = 0;
	free(ptr);
}

struct time_test_obj malloc_obj = {
	.fun_name = "MALLOC",
	.allocation = all_malloc,
	.deallocation = deall_free
};

struct time_test_obj calloc_obj = {
	.fun_name = "CALLOC",
	.allocation = all_calloc,
	.deallocation = deall_free
};

struct time_test_obj alloca_obj = {
	.fun_name = "ALLOCA",
	.allocation = all_alloca,
	.deallocation = deall_none
};

int main(int argc, char **argv)
{	
	int max_range = 64;
	int repeating = 100;
	unsigned long long meas_time = get_av_meas_time(100);

	struct time_test_obj *test_obj_arr[] = {&malloc_obj, 
						&calloc_obj, 
						&alloca_obj, 
						NULL};

	printf("MEASURE TIME AVARAGE = %llu\n\n", meas_time);

	for (int i = 0; test_obj_arr[i] != NULL; i++) {
		for (int j = 0; j < 2; j+=1) {
			do_mem_range_test(test_obj_arr[i], max_range, 
						repeating, meas_time, j);
			sleep(1);
		}
	}

	return 0;
}
