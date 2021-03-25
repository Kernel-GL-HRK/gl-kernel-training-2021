// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BUF_LEN 256

int main(void)
{

	char buf[BUF_LEN] = {0};

	time_t rawtime = time(NULL);

	if (rawtime == -1) {
		puts("The time() function failed");
		return 1;
	}

	struct tm *ptm = localtime(&rawtime);

	if (ptm == NULL) {
		puts("The localtime() function failed");
		return 1;
	}

	strftime(buf, BUF_LEN, "Today is %A", ptm);
	puts(buf);

	memset(buf, 0, BUF_LEN);
	strftime(buf, BUF_LEN, "The month is %B", ptm);
	puts(buf);

	memset(buf, 0, BUF_LEN);
	strftime(buf, BUF_LEN, "Today is %-d day of %B", ptm);
	puts(buf);

	memset(buf, 0, BUF_LEN);
	strftime(buf, BUF_LEN, "Today is %-j day of %G", ptm);
	puts(buf);

	memset(buf, 0, BUF_LEN);
	strftime(buf, BUF_LEN, "Today is %-W week of %G", ptm);
	puts(buf);

	memset(buf, 0, BUF_LEN);
	strftime(buf, BUF_LEN, "The time is %T", ptm);
	puts(buf);

	memset(buf, 0, BUF_LEN);
	strftime(buf, BUF_LEN, "The date is %D", ptm);
	puts(buf);

	return 0;

}
