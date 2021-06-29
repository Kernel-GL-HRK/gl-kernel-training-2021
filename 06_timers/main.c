// SPDX-License-Identifier: GPL-3.0
/**/
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int print_clock_gettime(clockid_t clock)
{
	const int bufsize = 50;
	char buf[bufsize];
	struct timespec now;
	struct tm ts;

	memset(buf, '\0', bufsize);
	if (clock_gettime(clock, &now) == -1) {
		perror("clock_gettime failure");
		exit(EXIT_FAILURE);
	}
	gmtime_r(&now.tv_sec, &ts);
	strftime(buf, bufsize, "%Y-%m-%d Time: %H:%M:%S.", &ts);
	sprintf(buf, "%s%09lu", buf, now.tv_nsec);
	printf("%s\n", buf);
	if (clock_getres(clock, &now) == -1) {
		perror("clock_getres failure");
		exit(EXIT_FAILURE);
	}
	printf("  resolution: %10jd.%09ld\n\n",
		(intmax_t) now.tv_sec, now.tv_nsec);
	return 0;
}

int main(void)
{
	printf("=============================\n");
	printf("CLOCK_REALTIME time: ");
	print_clock_gettime(CLOCK_REALTIME);
    #ifdef CLOCK_TAI
		printf("=============================\n");
		printf("CLOCK_TAI time: ");
		print_clock_gettime(CLOCK_TAI);
    #endif
	return 0;
}

