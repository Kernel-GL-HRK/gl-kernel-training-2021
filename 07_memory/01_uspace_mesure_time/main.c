#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MEM_MAX_PWR 64
#define AMOUNT_TESTS 10
#define NSEC_IN_SEC 1000000000
#define TABLE_HEADER	"|----------------------------------------------------------------------------------------|\n"\
			"|                                    Testing %s()                                    |\n"\
			"|----------|--------------------------------------|--------------------------------------|\n"\
			"|          |         Alloc time sec               |          Free time sec               |\n"\
			"| Buf size |------------|------------|------------|------------|------------|------------|\n"\
			"|          |    Min     |   Average  |    Max     |    Min     |   Average  |    Max     |\n"\
			"|----------|------------|------------|------------|------------|------------|------------|\n"
#define TABLE_DELIMETER "|----------|------------|------------|------------|------------|------------|------------|\n"
#define TABLE_ROW	"|%10lu|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|%02ld.%09ld|\n"

#define SET_MIN_SPEC(min, spec) do {\
		if ((!min.tv_sec && !min.tv_nsec) ||\
				(min.tv_sec > spec.tv_sec)) {\
			memcpy(&min, &spec, sizeof(spec));\
			break;\
		} \
		if (min.tv_nsec > spec.tv_nsec)\
			min.tv_nsec = spec.tv_nsec;\
	} while (0)

#define SET_MAX_SPEC(max, spec) do {\
		if ((!max.tv_sec && !max.tv_nsec) ||\
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

typedef enum {
	MALLOC_TYPE = 0,
	CALLOC_TYPE,
	ALLOCA_TYPE
} alloc_type_t;

static void *memory_alloc_check_time(unsigned long size, alloc_type_t type,
		struct timespec *ret_spec)
{
	void *ptr;
	long nsec;
	struct timespec spec, cmp_spec;

	if (clock_gettime(CLOCK_REALTIME, &spec) == -1) {
		perror("The clock_gettime() function failed");
		return NULL;
	}

	switch (type) {
	case MALLOC_TYPE:
		ptr = malloc(size);
		break;
	case CALLOC_TYPE:
		ptr = calloc(1, size);
		break;
	case ALLOCA_TYPE:
		ptr = alloca(size);
		break;
	default:
		ptr = NULL;
		break;
	}

	if (!ptr) {
		perror("memory_alloc_check_time() error");
		return NULL;
	}

	if (clock_gettime(CLOCK_REALTIME, &cmp_spec) == -1) {
		perror("The clock_gettime() function failed");
		if (type != ALLOCA_TYPE)
			free(ptr);
		return NULL;
	}

	nsec = cmp_spec.tv_nsec - spec.tv_nsec;
	if (nsec < 0) {
		nsec += NSEC_IN_SEC;
		cmp_spec.tv_sec -= 1;
	}

	ret_spec->tv_sec = cmp_spec.tv_sec - spec.tv_sec;
	ret_spec->tv_nsec = nsec;

	return ptr;
}

static void memory_free_check_time(void *ptr, alloc_type_t type,
		struct timespec *ret_spec)
{
	long nsec;
	struct timespec spec, cmp_spec;

	if (clock_gettime(CLOCK_REALTIME, &spec) == -1) {
		perror("The clock_gettime() function failed");
		return;
	}

	switch (type) {
	case MALLOC_TYPE:
	case CALLOC_TYPE:
		free(ptr);
		break;
	case ALLOCA_TYPE:
		/* Stack memory no need to be free */
		return;
	default:
		break;
	}

	if (clock_gettime(CLOCK_REALTIME, &cmp_spec) == -1) {
		perror("The clock_gettime() function failed");
		return;
	}

	nsec = cmp_spec.tv_nsec - spec.tv_nsec;
	if (nsec < 0) {
		nsec += NSEC_IN_SEC;
		cmp_spec.tv_sec -= 1;
	}

	ret_spec->tv_sec = cmp_spec.tv_sec - spec.tv_sec;
	ret_spec->tv_nsec = nsec;
}

static int memory_check_size(alloc_type_t type, unsigned long size)
{
	int i;
	void *ptr;
	struct timespec spec, arr_spec[AMOUNT_SPEC] = {0};

	for (i = 0; i < AMOUNT_TESTS; i++) {
		ptr = memory_alloc_check_time(size, type, &spec);

		if (!ptr)
			return -1;
		
		ADD_TO_AVRG_SPEC(arr_spec[ALLOC_AVRG_SPEC], spec);
		SET_MIN_SPEC(arr_spec[ALLOC_MIN_SPEC], spec);
		SET_MAX_SPEC(arr_spec[ALLOC_MAX_SPEC], spec);

		if (type != ALLOCA_TYPE) {
			memory_free_check_time(ptr, type, &spec);
			ADD_TO_AVRG_SPEC(arr_spec[FREE_AVRG_SPEC], spec);
			SET_MIN_SPEC(arr_spec[FREE_MIN_SPEC], spec);
			SET_MAX_SPEC(arr_spec[FREE_MAX_SPEC], spec);
		}
	}

	printf(TABLE_ROW, size, arr_spec[ALLOC_MIN_SPEC].tv_sec,
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

static void memory_test(alloc_type_t type)
{
	unsigned long i;

	for (i = 0; i < MEM_MAX_PWR; i++) {
		if (memory_check_size(type, (unsigned long)1 << i) == -1)
			break;

		if (i && memory_check_size(type, (unsigned long)(1 << i) + 1)
				== -1)
			break;
	}
}

int main(void)
{
	printf(TABLE_HEADER, "malloc");
	memory_test(MALLOC_TYPE);
	printf(TABLE_HEADER, "calloc");
	memory_test(CALLOC_TYPE);

	/* alloca() need to be the last test because of segmentation fault */
	printf(TABLE_HEADER, "alloca");
	memory_test(ALLOCA_TYPE);

	return 0;
}
