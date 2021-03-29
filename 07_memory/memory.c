// SPDX-License-Identifier: GPL-3.0
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <alloca.h>
#include <sys/resource.h>
#include <string.h>
#include "memory.h"

struct STAT_ELEM {
long long t_alloc;
long long t_free;
double num;
};

struct STAT_RES {
long long t_alloc_max;
long long t_alloc_min;
long long t_alloc_sum;
long long t_free_max;
long long t_free_min;
long long t_free_sum;
int cnt;
int order;
double num;
};

long long delta_time(struct timespec start, struct timespec stop)
{
	struct timespec delta;

	delta.tv_sec = (intmax_t)(stop.tv_sec-start.tv_sec);
	delta.tv_nsec = stop.tv_nsec-start.tv_nsec;
	if (delta.tv_nsec < 0)
		delta.tv_nsec += 1000000000;
	return (long long)(delta.tv_sec*1000000000+delta.tv_nsec);
}

void get_cur_time(struct timespec *cur_time)
{
	if (clock_gettime(CLOCK_MONOTONIC, cur_time) == -1) {
		perror("clock_gettime failure");
		exit(EXIT_FAILURE);
	}
}

void alloc_mem(int * (*fun) (struct STAT_ELEM *), struct STAT_ELEM *st_el)
{
	int *ptr;
	static struct timespec start, stop;

	ptr = (int *)fun(st_el);
	if (ptr != NULL) {
		get_cur_time(&start);
		free(ptr);
		get_cur_time(&stop);
		st_el->t_free = delta_time(start, stop);
	}
}

void print_header(char *file_name, char *header)
{
	FILE *fp;

	fp = fopen(file_name, "a+");
	if (!fp) {
		perror("file output failure");
		return;
	}
	fputs(header, fp);
	fclose(fp);
}

void print_stat(char *file_name, struct STAT_RES st_res)
{
	FILE *fp;

	fp = fopen(file_name, "a+");
	if (!fp) {
		perror("file output failure");
		return;
	}
	fprintf(fp, "======================================================\n");
	fprintf(fp, "ORDER: 2^%-2d BUF_SIZE: %-20.0lf\n",
		st_res.order, st_res.num);
	fprintf(fp, "ALLOC_TIME (nsec): ");
	fprintf(fp, "MAX = %-20lld AVG = %-20.0lf MIN = %-20lld\n",
		st_res.t_alloc_max,
		(((double)st_res.t_alloc_sum) / ((double)st_res.cnt)),
		st_res.t_alloc_min);
	fprintf(fp, "FREE_TIME  (nsec): ");
	fprintf(fp, "MAX = %-20lld AVG = %-20.0lf MIN = %-20lld\n",
		st_res.t_free_max,
		(((double)st_res.t_free_sum) / ((double)st_res.cnt)),
		st_res.t_free_min);
	fclose(fp);
}

void update_statistic(struct STAT_ELEM st_item, struct STAT_RES *statistic)
{
	statistic->t_alloc_max = (st_item.t_alloc < statistic->t_alloc_max) ?
		statistic->t_alloc_max : st_item.t_alloc;
	statistic->t_alloc_min = (statistic->t_alloc_min) ?
		statistic->t_alloc_min : st_item.t_alloc;
	statistic->t_alloc_min = (st_item.t_alloc > statistic->t_alloc_min) ?
		statistic->t_alloc_min : st_item.t_alloc;
	statistic->t_alloc_sum += st_item.t_alloc;
	statistic->t_free_max = (st_item.t_free < statistic->t_free_max) ?
		statistic->t_free_max : st_item.t_free;
	statistic->t_free_min = (statistic->t_free_min) ?
		statistic->t_free_min : st_item.t_free;
	statistic->t_free_min = (st_item.t_free > statistic->t_free_min) ?
		statistic->t_free_min : st_item.t_free;
	statistic->t_free_sum += st_item.t_free;
	statistic->cnt += 1;
}

void set_elem(struct STAT_ELEM *st_item, struct STAT_RES statistic)
{
	memset(st_item, 0, sizeof(*st_item));
	st_item->num = statistic.num;
}

int check_mem(char *fname, int * (*fun) (struct STAT_ELEM *))
{
	int order;
	int i;
	const int iorder = 64;
	const int istat = 5;
	struct STAT_ELEM stat_element;
	struct STAT_RES stat, stat_plus_1;


	for (order = 0; order <= iorder; order++) {
		memset(&stat, 0, sizeof(stat));
		memset(&stat_plus_1, 0, sizeof(stat_plus_1));
		stat.num = pow((double)2, (double)order);
		stat_plus_1.num = stat.num + 1.0;
		stat.order = stat_plus_1.order = order;
		for (i = 0; i < istat; i++) {
			set_elem(&stat_element, stat);
			alloc_mem(fun, &stat_element);
			update_statistic(stat_element, &stat);
			set_elem(&stat_element, stat_plus_1);
			alloc_mem(fun, &stat_element);
			update_statistic(stat_element, &stat_plus_1);
		}
		print_stat(fname, stat);
		print_stat(fname, stat_plus_1);
	}
	return 0;
}

int *malloc_buf(struct STAT_ELEM *st_el)
{
	int *ptr;
	struct timespec start, stop;

	if ((st_el->num * sizeof(int)) >= SIZE_MAX)
		return NULL;
	get_cur_time(&start);
	ptr = (int *)malloc(st_el->num * sizeof(int));
	get_cur_time(&stop);
	if (ptr == NULL)
		return NULL;
	st_el->t_alloc = delta_time(start, stop);
	return ptr;
}

int *calloc_buf(struct STAT_ELEM *st_el)
{
	int *ptr;
	struct timespec start, stop;

	if (st_el->num >= SIZE_MAX)
		return NULL;
	get_cur_time(&start);
	ptr = (int *)calloc(st_el->num, sizeof(int));
	get_cur_time(&stop);
	if (ptr == NULL)
		return NULL;
	st_el->t_alloc = delta_time(start, stop);
	return ptr;
}

int *alloca_buf(struct STAT_ELEM *st_el)
{
	int *ptr;
	struct rlimit rl;
	struct timespec start, stop;

	if ((getrlimit(RLIMIT_STACK, &rl) != 0) ||
		((st_el->num * sizeof(int)) >= rl.rlim_cur) ||
		((st_el->num * sizeof(int)) >= SIZE_MAX))
		return NULL;
	get_cur_time(&start);
	ptr = (int *)alloca(st_el->num * sizeof(int));
	get_cur_time(&stop);
	if (ptr != NULL)
		st_el->t_alloc = delta_time(start, stop);
	return NULL;
}

int check_malloc(char *fname)
{
	print_header(fname, "\nMALLOC TABLE\n");
	check_mem(fname, malloc_buf);
	return 0;
}

int check_calloc(char *fname)
{
	print_header(fname, "\nCALLOC TABLE\n");
	check_mem(fname, calloc_buf);
	return 0;
}
int check_alloca(char *fname)
{
	print_header(fname, "\nALLOCA TABLE\n");
	check_mem(fname, alloca_buf);
	return 0;
}
