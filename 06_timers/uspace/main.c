// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
	struct timespec timespec_var;
	char buffer[200] = {'\0'};

	timespec_get(&timespec_var, TIME_UTC);
	strftime(buffer, sizeof(buffer), "%T",
		gmtime(&timespec_var.tv_sec));
	printf("Time now in UTC: %s.%09ld UTC\n", buffer,
		timespec_var.tv_nsec);

	return 0;
}
