#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#define NSEC_IN_SEC     1e9
#define MAX_BUF_SIZE    64
#define NUM_OF_ITER 	10

#define POW2(x)     (1LL << (x))

#define MIN(x, y) x > y ? y : x
#define MAX(x, y) x > y ? x : y

uint64_t get_diff(struct timespec t1, struct timespec t2)
{
	uint64_t sec_diff;
	uint64_t nsec_diff;

	sec_diff = t1.tv_sec - t2.tv_sec;
	nsec_diff = t1.tv_nsec - t2.tv_nsec;

	if (nsec_diff < 0) {
		nsec_diff += NSEC_IN_SEC;
		sec_diff--;
	}

	return (sec_diff * NSEC_IN_SEC) + nsec_diff;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct alloc_free_time {
	uint64_t alloc_time;
	uint64_t free_time;
};

enum mem_func {
	MALLOC_F = 0,
	CALLOC_F,
	ALLOCA_F
};

bool check_alloca_time(uint64_t size, struct alloc_free_time *t)
{
	uint8_t *tmp;
	struct timespec t1, t2;
	struct rlimit stack_size;
	t->free_time = 0;

	getrlimit(RLIMIT_STACK, &stack_size);

	if(stack_size.rlim_cur < size)
		return false;

	clock_gettime(CLOCK_REALTIME, &t2);
	tmp = (uint8_t*)alloca(size);
	clock_gettime(CLOCK_REALTIME, &t1);
	t->alloc_time = get_diff(t1, t2);

	if (tmp == NULL)
		return false;

	return true;
}


bool check_time(uint64_t size, struct alloc_free_time *t, enum mem_func func)
{
	uint8_t *tmp;
	struct timespec t1, t2;

	clock_gettime(CLOCK_REALTIME, &t2);

	switch(func) {
		case MALLOC_F: tmp = malloc(size * sizeof(*tmp));   break;
		case CALLOC_F: tmp = calloc(size, sizeof(*tmp));    break;
		default: tmp = NULL;                                break;
	}

	clock_gettime(CLOCK_REALTIME, &t1);

	t->alloc_time = get_diff(t1, t2);

	if (tmp == NULL)
		return false;

	clock_gettime(CLOCK_REALTIME, &t2);
	free(tmp);
	clock_gettime(CLOCK_REALTIME, &t1);
	t->free_time = get_diff(t1, t2);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void test_and_print(char *text, uint8_t adding, enum mem_func func)
{
	struct alloc_free_time *t = malloc(sizeof(*t));
	struct alloc_free_time *t_min = malloc(sizeof(*t));
	struct alloc_free_time *t_max = malloc(sizeof(*t));
	struct alloc_free_time *t_aver = malloc(sizeof(*t_aver));
	uint64_t size;
	bool flag;

	t_min->alloc_time = 0;
	t_min->free_time = 0;
	t_aver->alloc_time = 0;
	t_aver->free_time = 0;
	t_max->alloc_time = 0;
	t_max->free_time = 0;

	printf("************************************************\n");
	printf("*****************%s*****************\n", text);
	printf("****************size = (2^x)+%d******************\n", adding);
	printf("************************************************\n");
	printf("iter\tmin_alloc(ns)\taver_alloc(ns)\tmax_alloc(ns)\t|\tmin_free(ns)\taver_free(ns)\tmax_free(ns)\tsize(bytes)\n");

	for (uint8_t i = 0; i < MAX_BUF_SIZE; i++) {

		size = POW2(i);


		if (func == ALLOCA_F)
			flag = check_alloca_time(size, t);
		else
			flag = check_time(size, t, func);

		if (flag == false) {
			printf("Can't allocate:(\n");
			break;
		}

		t_min->alloc_time = t->alloc_time;
		t_min->free_time = t->free_time;
		t_aver->alloc_time += t->alloc_time;
		t_aver->free_time += t->free_time;
		t_max->alloc_time = t->alloc_time;
		t_max->free_time = t->free_time;

		uint8_t j = NUM_OF_ITER;
		while(j--) {
			if (func == ALLOCA_F)
				flag = check_alloca_time(size, t);
			else
				flag = check_time(size, t, func);

			t_min->alloc_time = MIN(t_min->alloc_time, t->alloc_time);
			t_min->free_time = MIN(t_min->free_time, t->free_time);
			t_aver->alloc_time += t->alloc_time;
			t_aver->free_time += t->free_time;
			t_max->alloc_time = MAX(t_max->alloc_time, t->alloc_time);
			t_max->free_time = MAX(t_max->free_time, t->free_time);
		}

		t_aver->free_time /= NUM_OF_ITER;
		t_aver->alloc_time /= NUM_OF_ITER;

		
		printf("%d\t%ld\t\t%ld\t\t%ld\t\t|\t%ld\t\t%ld\t\t%ld\t\t%lu\n", i, 
					t_min->alloc_time, t_aver->alloc_time, t_max->alloc_time, 
					t_min->free_time, t_aver->free_time, t_max->free_time, size);

	}

	free(t);
	free(t_aver);
	free(t_max);
	free(t_min);
}

int main(void) 
{   
	test_and_print("MALLOC TESTING", 0, MALLOC_F);

	test_and_print("CALLOC TESTING", 0, CALLOC_F);

	test_and_print("MALLOC TESTING", 1, MALLOC_F);

	test_and_print("CALLOC TESTING", 1, CALLOC_F);

	test_and_print("ALLOCA TESTING", 0, ALLOCA_F);

	return 0;
}
