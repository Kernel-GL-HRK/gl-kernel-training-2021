#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>

#define NSEC_IN_SEC     1000000000LL
#define MAX_BUF_SIZE    64

#define POW2(x)     (1LL << (x))

uint64_t get_diff(struct timespec t1, struct timespec t2)
{
	if (t1.tv_sec == t2.tv_sec)
		return (t1.tv_nsec - t2.tv_nsec);

	uint64_t tmp_nsec = (NSEC_IN_SEC + t1.tv_nsec) - t2.tv_nsec;

	uint64_t tmp_sec = tmp_nsec / NSEC_IN_SEC;
	tmp_sec += (t1.tv_sec - t2.tv_sec - 1);

	return (tmp_sec * NSEC_IN_SEC) + (tmp_nsec % NSEC_IN_SEC);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct alloc_free_time {
	uint64_t alloc_time;
	uint64_t free_time;
	uint8_t *ptr;
};

enum mem_func {
	MALLOC_F = 0,
	CALLOC_F,
	ALLOCA_F
};

bool check_time(uint64_t size, struct alloc_free_time *t, enum mem_func func)
{
	uint8_t *tmp;
	struct timespec t1, t2;

	clock_gettime(CLOCK_REALTIME, &t2);

	switch(func) {
		case MALLOC_F: tmp = malloc(size * sizeof(*tmp));   break;
		case CALLOC_F: tmp = calloc(size, sizeof(*tmp));    break;
		case ALLOCA_F: tmp = alloca(sizeof(*tmp) * size);   break;
		default: tmp = NULL;                                break;
	}

	clock_gettime(CLOCK_REALTIME, &t1);

	t->ptr = tmp;

	if (tmp == NULL)
		return false;

	t->alloc_time = get_diff(t1, t2);

	if (func == ALLOCA_F) {
		t->free_time = 0;
		goto ret;    
	}

	clock_gettime(CLOCK_REALTIME, &t2);
	free(tmp);
	clock_gettime(CLOCK_REALTIME, &t1);
	t->free_time = get_diff(t1, t2);

ret:
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void test_and_print(char *text, uint8_t adding, enum mem_func func)
{
	struct alloc_free_time *t = malloc(sizeof(*t));
	struct alloc_free_time *t_aver = malloc(sizeof(*t_aver));
	uint64_t size;
	bool flag;

	t_aver->alloc_time = 0;
	t_aver->free_time = 0;

	printf("************************************************\n");
	printf("*****************%s*****************\n", text);
	printf("****************size = (2^x)+%d******************\n", adding);
	printf("************************************************\n");


	for (uint8_t i = 0; i < MAX_BUF_SIZE; i++) {
		
		printf("Iter No%d\n", i);

		size = POW2(i);

		flag = check_time(size, t, func);

		if(flag == false) {
			printf("Can't allocate:(\n");
			break;
		}

		t_aver->alloc_time += t->alloc_time;
		t_aver->free_time += t->free_time;

		printf("Start adress is %p\n", t->ptr);
		printf("Alloc time = %ldns\t", t->alloc_time);
		printf("Free time = %ldns\n", t->free_time);
	}

	printf("Aver allocating time is %ldns\t", t_aver->alloc_time / MAX_BUF_SIZE);
	printf("Aver freeing time is %ldns\n", t_aver->free_time / MAX_BUF_SIZE);

	free(t);
	free(t_aver);
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
