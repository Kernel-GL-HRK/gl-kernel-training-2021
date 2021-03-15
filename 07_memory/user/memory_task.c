// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <time.h>
#include <unistd.h>

#define ALLOC_SIZE_BASE 2
#define ALLOC_SIZE_MAX_EXPONENT (64 - 1)

#define TIME_MEASURE(expr) \
	do { \
		timespec_get(&_prev_ts, TIME_UTC); \
		{ \
			expr; \
		} \
		timespec_get(&_ts, TIME_UTC); \
		sec_diff = _ts.tv_sec - _prev_ts.tv_sec; \
		nsec_diff = _ts.tv_nsec - _prev_ts.tv_nsec; \
		if (nsec_diff < 0) { \
			nsec_diff += 1e9; \
			sec_diff--; \
		} \
	} while (0)

#define TIME_MEASURED(precision) \
	((nsec_diff / (1e9 / precision)) + (sec_diff * precision))

enum alloc_func_t {
	_MALLOC = 0,
	_CALLOC,
	_ALLOCA,
	_ALLOC_FUNC_CNT
};

static time_t sec_diff, nsec_diff;
static struct timespec _ts, _prev_ts;

static unsigned long _pow(unsigned int base, unsigned int exponent)
{
	unsigned long result = 1;

	if (exponent == 0)
		return result;

	while (exponent--)
		result *= base;

	return result;
}

static char *get_alloc_func_name(enum alloc_func_t alloc_func)
{
	switch (alloc_func) {
	case _MALLOC:
		return "malloc";
	case _CALLOC:
		return "calloc";
	case _ALLOCA:
		return "alloca";
	}
	return NULL;
}

static inline void *alloc(enum alloc_func_t alloc_func, size_t size)
{
	switch (alloc_func) {
	case _MALLOC:
		return malloc(size);
	case _CALLOC:
		return calloc(size, sizeof(char));
	case _ALLOCA:
		return alloca(size);
	}
	return NULL;
}

static inline void dealloc(enum alloc_func_t alloc_func, void *_ptr)
{
	switch (alloc_func) {
	case _MALLOC:
	case _CALLOC:
		return free(_ptr);
	}
}

void measure_alloc_time(enum alloc_func_t alloc_func)
{
	unsigned int i;
	char *alloc_func_name = get_alloc_func_name(alloc_func);

	printf("\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\t\tTesting %s() function\n", alloc_func_name);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	printf(" #\t%s()\tfree()\t\tsize\n", alloc_func_name);

	for (i = 0; i < ALLOC_SIZE_MAX_EXPONENT; ++i) {
		void *_ptr;
		double alloc_time, free_time;
		unsigned long alloc_size = _pow(ALLOC_SIZE_BASE, i);

		TIME_MEASURE(_ptr = alloc(alloc_func, alloc_size));
		alloc_time = TIME_MEASURED(1e6);

		if (_ptr == NULL) {
			printf("%s(%d^%u)(%.1f Gb) failed!\n",
				alloc_func_name, ALLOC_SIZE_BASE, i,
				alloc_size / 1024.f / 1024.f / 1024.f);
			break;
		}

		TIME_MEASURE(dealloc(alloc_func, _ptr));
		free_time = TIME_MEASURED(1e6);

		printf("%02u\t%.3f us\t%.3f us\t%d^%02u(%lu) bytes\n", i + 1,
			alloc_time, (alloc_func == _ALLOCA) ? -1 : free_time,
			ALLOC_SIZE_BASE, i, alloc_size);
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

int main(int argc, char **argv)
{
	unsigned int i;

	for (i = 0; i < _ALLOC_FUNC_CNT; ++i)
		measure_alloc_time(i);

	return 0;
}
