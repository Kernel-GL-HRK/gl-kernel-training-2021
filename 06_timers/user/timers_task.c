// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <stdio.h>
#include <time.h>

static void pr_time(struct timespec *_ts)
{
	struct tm *_tm = localtime((time_t *)&_ts->tv_sec);

	printf("%02d/%02d/%04d %02d:%02d:%02d.%ld\n",
					_tm->tm_mday, _tm->tm_mon + 1,
					_tm->tm_year + 1900, _tm->tm_hour,
					_tm->tm_min, _tm->tm_sec, _ts->tv_nsec);
}

int main(int argc, char **argv)
{
	struct timespec _ts;

	timespec_get(&_ts, TIME_UTC);
	pr_time(&_ts);

	return 0;
}
