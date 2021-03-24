// SPDX-License-Identifier: MIT and GPL
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define NUM_OF_REPETITION	10
#define MAX_BUF_SIZE		64
#define TIME_PR_TEMPLATE "\t%03f us"

#define POW2(x)     (1LL << (x))

struct alloc_free_time {
	double alloc_time;
	double free_time;
	uint8_t *ptr;
};

enum mem_func {
	MALLOC_F = 0,
	CALLOC_F,
	ALLOCA_F
};

bool check_time(uint8_t pow_of_2, struct alloc_free_time *t, enum mem_func func)
{

void *tmp_ptr;
clock_t t1, t2;
uint64_t size = POW2(pow_of_2);

t1 = clock();

switch (func) {

case MALLOC_F:
	tmp_ptr = (uint8_t *) malloc(size * sizeof(uint8_t));
	break;

case CALLOC_F:
	tmp_ptr = (uint8_t *) calloc(size, sizeof(uint8_t));
	break;

case ALLOCA_F:
	tmp_ptr = (uint8_t *) alloca(size);
	break;

default:
	tmp_ptr = NULL;
	break;
}

t2 = clock();

	t->ptr = tmp_ptr;

	if (tmp_ptr == NULL)
		return false;

t->alloc_time = ((double)(t2 - t1))/CLOCKS_PER_SEC;

t1 = clock();

switch (func) {

case MALLOC_F:
case CALLOC_F:
	free(tmp_ptr);
	break;

case ALLOCA_F:
	break;

default:
	tmp_ptr = NULL;
	break;
}

t2 = clock();

if (func == ALLOCA_F)
	t->free_time = 0.0;
else
	t->free_time = ((double)(t2 - t1))/CLOCKS_PER_SEC;

return true;

}


void test_and_print(char *text, enum mem_func func)
{
uint8_t i;
struct alloc_free_time *t = calloc(1, sizeof(*t));
struct alloc_free_time *t_aver = calloc(1, sizeof(*t_aver));
struct alloc_free_time *t_max = calloc(1, sizeof(*t_aver));
struct alloc_free_time *t_min = calloc(1, sizeof(*t_aver));
bool flag;

	t_aver->alloc_time = 0;
	t_aver->free_time = 0;

	printf("Test %s, number of runs: %u\n", text, NUM_OF_REPETITION);
	printf("\tsize\t%*s%s\t%*s%s\t%*s%s\t%*s%s\t%*s%s\t%*s%s",
		2, "", "alloc, max", 2, "", "alloc, min",
		2, "", "alloc, avg", 3, "", "free, max",
		3, "", "free, min", 3, "", "free, avg");
	printf("\n");
	for (i = 0; i < MAX_BUF_SIZE; i++) {

		uint8_t iter = NUM_OF_REPETITION;

		flag = check_time(i, t, func);

		if (flag == false) {
			printf("Can't allocate:(\n");
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

			t_max->alloc_time = fmax(t_max->alloc_time,
						t->alloc_time);
			t_max->free_time = fmax(t_max->free_time, t->free_time);
			t_min->alloc_time = fmax(t_min->alloc_time,
							t->alloc_time);
			t_min->free_time = fmax(t_min->free_time, t->free_time);


		} while (--iter);
	printf("\t2^%02d"
		TIME_PR_TEMPLATE TIME_PR_TEMPLATE
		TIME_PR_TEMPLATE TIME_PR_TEMPLATE
		TIME_PR_TEMPLATE TIME_PR_TEMPLATE
		"\n",
		i,
		t_max->alloc_time, t_aver->alloc_time / (double) MAX_BUF_SIZE,
		t_min->alloc_time, t_max->free_time,
		t_aver->free_time / (double) MAX_BUF_SIZE, t_min->free_time);

		}

printf("\n");
free(t);
free(t_aver);
}


int main(void)
{
printf("'task_05'\n");

test_and_print("MALLOC", MALLOC_F);

test_and_print("CALLOC", CALLOC_F);

test_and_print("ALLOCA", ALLOCA_F);

return 0;
}
