// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <malloc.h>
#include <ctype.h>

#define EINVAL 22
#define ENOMEM 12

#define NSEC_PER_SEC 1000000000ll

#define MEMTEST_ATEMPTS_POWER (4)
#define MEMTEST_ATEMPTS (1<<MEMTEST_ATEMPTS_POWER)

#define MEMTEST_SIZE(x) (1ll<<(x))
#define MEMTEST_LINMIT	(64)

enum {
	MEMTS_MALLOC = 0,
	MEMTS_CALLOC,
	MEMTS_ALLOCA,
	MEMTS_MAX,
};

struct mem_ops {
	char head_str[16];
	void* (*space_alloc)(size_t size);
	void (*space_free)(void *ptr);
};

struct time_stats {
	__uint64_t min_ns;
	__uint64_t max_ns;
	__uint64_t average_ns;
};

static struct time_stats alloc_stats = {__UINT64_MAX__, 0, 0};
static struct time_stats free_stats = {__UINT64_MAX__, 0, 0};
static __uint64_t time_offset;

static void update_stats(struct time_stats *stats, __uint64_t time_ns)
{
	time_ns = (time_ns > time_offset) ? (time_ns - time_offset) : 0;

	stats->average_ns += time_ns;

	if (time_ns > stats->max_ns)
		stats->max_ns = time_ns;
	if (time_ns < stats->min_ns)
		stats->min_ns = time_ns;
}

static void process_stats(struct time_stats *stats, const char *msg)
{
	stats->average_ns >>= MEMTEST_ATEMPTS_POWER;

	printf("%s: min [%lu ns], max [%lu ns], average [%lu ns]\n",
		msg, stats->min_ns, stats->max_ns, stats->average_ns);

	stats->average_ns = 0;
	stats->min_ns = __UINT64_MAX__;
	stats->max_ns = 0;

}

__uint64_t delta_ns(const struct timespec *t_from,
			const struct timespec *t_to)
{
	return  ((t_to->tv_sec*NSEC_PER_SEC) + t_to->tv_nsec) -
		((t_from->tv_sec*NSEC_PER_SEC) + t_from->tv_nsec);
}


static int memtest(__uint8_t stage, struct mem_ops *test_func)
{
	__uint8_t indx = 0;
	char *buffer = NULL;
	struct timespec time_stamp[2] = {0};

	if (test_func == NULL) {
		printf("%s, mem_ops pointer is NULL\n",
		       __func__);
		return -EINVAL;
	}

	// pull testes functions to cashe
	buffer = test_func->space_alloc(1);
	if (buffer == NULL) {
		printf("%s, failed to cashe function %s\n",
		       __func__, test_func->head_str);
		return -ENOMEM;
	}
	test_func->space_free(buffer);

	printf("Runing test with [%s] function at size: [2^%d] bytes\n",
		test_func->head_str,
		stage);

	while (indx < MEMTEST_ATEMPTS) {
		indx++;
		clock_gettime(CLOCK_MONOTONIC, &time_stamp[0]);
		buffer = test_func->space_alloc(MEMTEST_SIZE(stage));
		clock_gettime(CLOCK_MONOTONIC, &time_stamp[1]);

		if (buffer == NULL) {
			printf("%s, alloc failed at size: 2^%d bytes, atempt: %d\n",
			       __func__, stage, indx);
			return -ENOMEM;
		}

		update_stats(&alloc_stats,  delta_ns(&time_stamp[0], &time_stamp[1]));

		clock_gettime(CLOCK_MONOTONIC, &time_stamp[0]);
		test_func->space_free(buffer);
		clock_gettime(CLOCK_MONOTONIC, &time_stamp[1]);
		/* emd time meas */

		update_stats(&free_stats, delta_ns(&time_stamp[0], &time_stamp[1]));
	}
	process_stats(&alloc_stats, "[STATS] alloc");
	process_stats(&free_stats, "[STATS] free");

	return 0;
}

static void *my_calloc(size_t size)
{
	return calloc(size, 1);
}

static void *my_alloca(size_t size)
{
	return alloca(size);
}

static void my_freea(void *prt)
{
}

static void calibrate_time(void)
{
	__uint8_t iteration = MEMTEST_ATEMPTS;
	struct timespec time_stamp[2]  = {0};

	while (iteration--) {
		clock_gettime(CLOCK_MONOTONIC, &time_stamp[0]);
		clock_gettime(CLOCK_MONOTONIC, &time_stamp[1]);
		time_offset += delta_ns(&time_stamp[0], &time_stamp[1]);

}
	time_offset >>= MEMTEST_ATEMPTS_POWER;

	printf("%s, ktime dump call delta: %lu\n",
		__func__, time_offset);

}

int main(void)
{
	int res = 0;
	__uint8_t stage = 0;
	__uint8_t funk_indx;
	struct mem_ops memtest_func[MEMTS_MAX] = {
		[MEMTS_MALLOC] = {"MALLOC", &malloc, &free},
		[MEMTS_CALLOC] = {"CALLOC", &my_calloc, &free},
		[MEMTS_ALLOCA] = {"ALLOCA", &my_alloca, &my_freea},
	};

	calibrate_time();

	while (stage < MEMTEST_LINMIT) {
		for (funk_indx = 0;
		     funk_indx < MEMTS_MAX;
		     funk_indx++) {
			res = memtest(stage, &memtest_func[funk_indx]);
			if (res < 0)
				return res;
		}
		stage++;
	}

	return 0;
}
