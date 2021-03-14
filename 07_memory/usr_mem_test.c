#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>

#define NSEC_IN_SEC	1000000000LL
#define MAX_SIZE	64
#define LOOP_SIZE	10

#define POW2(X)     (1ULL << (X))
#define MIN(x, y) ((x < y)?x:y)
#define MAX(x, y) ((x > y)?x:y)

uint64_t get_diff(const struct timespec *t_start, const struct timespec *t_end)
{
	return  ((t_end->tv_sec*NSEC_IN_SEC) + t_end->tv_nsec) - 
		((t_start->tv_sec*NSEC_IN_SEC) + t_start->tv_nsec);
}

char* mem_func_name[] = {"malloc", "calloc", "alloca"};
struct live_mem_time {
	uint64_t alloc_time;
	uint64_t free_time;
};

enum alloc_func_name {
	MALLOC = 0,
	CALLOC,
	ALLOCA,
	NUM_FN
};

enum test_case {
	TWO_POW_X = 0,
	TWO_POW_X_PLUS_ONE,
	NUM_TC
};

int get_time(uint64_t size, struct live_mem_time *t_live_mem, enum alloc_func_name func)
{
	struct timespec t_start, t_stop;
	char *mem_ptr;

	clock_gettime(CLOCK_MONOTONIC, &t_start);

	switch(func) {
		case MALLOC: 
			mem_ptr = malloc(size * sizeof(*mem_ptr)); 
			break;
		case CALLOC:
			mem_ptr = calloc(size, sizeof(*mem_ptr));
			break;
		case ALLOCA:
			mem_ptr = alloca(size * sizeof(*mem_ptr));
			break;
		default: mem_ptr = NULL;
		        break;
	}

	clock_gettime(CLOCK_MONOTONIC, &t_stop);

	if (mem_ptr == NULL)
		return 1;

	t_live_mem->alloc_time = get_diff(&t_start, &t_stop);

	if (func != ALLOCA) {
		clock_gettime(CLOCK_MONOTONIC, &t_start);
		free(mem_ptr);
		clock_gettime(CLOCK_MONOTONIC, &t_stop);
		t_live_mem->free_time = get_diff(&t_start, &t_stop);
	}

	return 0;
}


void run_test_mem(enum test_case test_num, enum alloc_func_name func)
{
	struct live_mem_time t_current = {0};
	uint64_t size = 0;
	uint64_t t_min_alloc = UINT64_MAX;
	uint64_t t_max_alloc = 0;
	uint64_t t_min_free = UINT64_MAX;
	uint64_t t_max_free = 0;
	int i = 0;
	int j = 0;

	printf("====== Run memory test allocation! ============================\n");
	printf("|\n");
	printf("| function call: %s\n", mem_func_name[func]);
	printf("| test case:     %s\n", (test_num == TWO_POW_X)?"2^x + 1":"2^x");
	printf("|\n");
	printf("===============================================================\n");

	for (i = 0; i < MAX_SIZE; i++) {

		t_min_alloc = UINT64_MAX;
		t_max_alloc = 0;
		t_min_free = UINT64_MAX;
		t_max_free = 0;

		size = (test_num == TWO_POW_X) ? POW2(i) : (POW2(i)+1);

		for (j = 0; j < LOOP_SIZE; j++) {
			if(get_time(size, &t_current, func)) {
				printf("| ### Memory cannot be allocated!\n");
				return;
			}

			t_min_alloc = MIN(t_current.alloc_time, t_min_alloc);
			t_max_alloc = MAX(t_current.alloc_time, t_max_alloc);
			t_min_free = MIN(t_current.free_time, t_min_free);
			t_max_free = MAX(t_current.free_time, t_max_free);
		}

		printf("| x: %2.d| alloc time: min: %8.lu(ns) average: %8.lu(ns)"
			"max: %8.lu(ns)| free time: min: %8.lu(ns)"
			"average: %8.lu(ns) max: %8.lu(ns)\n",
			i, t_min_alloc, (t_max_alloc + t_min_alloc)/2,
			t_max_alloc, t_min_free, (t_max_free + t_min_free)/2,
			t_max_free);
	}

}

int main(int argc, char* argv[]) 
{
	int i = 0;
	int j = 0;

	for(i = 0; i < NUM_TC; i++) {
		for (j = 0; j < NUM_FN-1; j++) {
			run_test_mem(i, j);
		}
	}

	if(argc > 1) {
		run_test_mem(TWO_POW_X, ALLOCA);
	}
	else {
		run_test_mem(TWO_POW_X_PLUS_ONE, ALLOCA);
	}

	return 0;
}
