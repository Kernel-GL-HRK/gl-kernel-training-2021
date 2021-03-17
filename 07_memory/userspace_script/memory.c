#include <stdio.h>
#include <time.h>
#include <malloc.h>       
#include <alloca.h>
#include <stdint.h>


#define X_MAX_VALUE		22
#define	REPETITIONS		16


enum Allocation_Methods_t {
	malloc_method = 0,
	calloc_method,
	alloca_method,
	allocation_methods_number
};

char m_names[allocation_methods_number][8] = {
		"malloc",
		"calloc",
		"alloca" };

int64_t calc_time_diff(struct timespec *start, struct timespec *end) {
  return (int64_t)(end->tv_sec - start->tv_sec) * (int64_t)1000000000UL
				+ (int64_t)(end->tv_nsec - start->tv_nsec);
}

void *allocate_testcase(int32_t method, int64_t size, int64_t *time_x)
{
	char *buf = NULL;
	struct timespec start_time = {0};
	struct timespec end_time = {0};

	switch (method) {
	case malloc_method:
		clock_gettime(0, &start_time);
		buf = malloc(size * sizeof(int));
		clock_gettime(0, &end_time);
		break;
	case calloc_method:
		clock_gettime(0, &start_time);
		buf = calloc(size, sizeof(int));
		clock_gettime(0, &end_time);
		break;
	case alloca_method:
		clock_gettime(0, &start_time);
		buf = alloca(size * sizeof(int));
		clock_gettime(0, &end_time);
		break;
	default:
		break;
	}
	
	*time_x = calc_time_diff(&start_time, &end_time);

	return buf;
}



void deallocate_testcase(void *buf, int32_t method, int64_t size, int64_t *time_x)
{
	struct timespec start_time = {0};
	struct timespec end_time = {0};

	switch (method) {
	case malloc_method:
		clock_gettime(0, &start_time);
		free(buf);
		clock_gettime(0, &end_time);
		break;
	case calloc_method:
		clock_gettime(0, &start_time);
		free(buf);
		clock_gettime(0, &end_time);
		break;
	case alloca_method:
		/* freeing is automatic */
		break;
	default:
		break;
	}
	
	*time_x = calc_time_diff(&start_time, &end_time);
}


int64_t find_max(const int64_t *arr, const int64_t length)
{
	int32_t i = 0;
	int64_t max = arr[0];

	for (i = 1; i < length; i++) {
		if (max < arr[i])
			max = arr[i];
	}

	return max;
}


int64_t find_average(const int64_t *arr, const int64_t length)
{
	int32_t i = 0;
	int64_t accum = 0;

	for (i = 0; i < length; i++)
		accum += arr[i];

	return (accum / length);
}


int64_t find_min(const int64_t *arr, const int64_t length)
{
	int32_t i = 0;
	int64_t min = arr[0];

	for (i = 1; i < length; i++) {
		if (min > arr[i])
			min = arr[i];
	}

	return min;
}


int32_t main(void)
{
	int32_t ret_value = 0;
	char *buf = NULL;
	int32_t method = malloc_method;
	int64_t size = 0;
	int64_t size_1 = 0;
	int32_t x = 0;
	int32_t j = 0;

	int64_t alloc_times[REPETITIONS] = {0};
	int64_t dealloc_times[REPETITIONS] = {0};


	printf("Lean back and wait for memory allocation test results!\n\n");

	for (method = malloc_method;
		 method < allocation_methods_number; method++) {

		printf("******************************** "
			"%s ********************************\n", m_names+method);
		printf("x   alloc (min\tavr\tmax)\t|"
			"dealloc (min\tavr\tmax), ns\n");

		for (x = 0; x < X_MAX_VALUE; x++) {
			size = 1ull << x;
			size_1 = size + 1u;

			for (j = 0; j < REPETITIONS; j++) {
				buf = allocate_testcase(method,
						size,
						&alloc_times[j]);
						deallocate_testcase(buf,
						method,
						size,
						&dealloc_times[j]);
			}
			printf("%llu\t%llu\t%llu\t%llu\t|"
				"\t%llu\t%llu\t%llu\n",
				size,
				find_min(alloc_times, REPETITIONS),
				find_average(alloc_times, REPETITIONS),
				find_max(alloc_times, REPETITIONS),
				find_min(dealloc_times, REPETITIONS),
				find_average(dealloc_times, REPETITIONS),
				find_max(dealloc_times, REPETITIONS));

			if (x != 0) {  //x = 0 -> size_1 = 2 (same as x = 1 -> size = 2)
				for (j = 0; j < REPETITIONS; j++) {
					buf = allocate_testcase(method,
							size_1,
							&alloc_times[j]);
							deallocate_testcase(buf,
							method,
							size_1,
							&dealloc_times[j]);
				}
				printf("%llu\t%llu\t%llu\t%llu\t|"
					"\t%llu\t%llu\t%llu\n",
					size_1,
					find_min(alloc_times, REPETITIONS),
					find_average(alloc_times, REPETITIONS),
					find_max(alloc_times, REPETITIONS),
					find_min(dealloc_times, REPETITIONS),
					find_average(dealloc_times, REPETITIONS),
					find_max(dealloc_times, REPETITIONS));
			}
		}
	}

	return ret_value;
}
