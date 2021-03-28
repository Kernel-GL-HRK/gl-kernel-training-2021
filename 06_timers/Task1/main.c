// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <stdio.h>
#include <time.h>

int main(void)
{
	time_t time_epoch_sec = 0;
	struct tm *cal_time;
	char *human_readable_time;

	time_epoch_sec = time(NULL);

	if (time_epoch_sec) {
		cal_time = localtime(&time_epoch_sec);
		if (cal_time != NULL) {
			human_readable_time = asctime(cal_time);

			if (human_readable_time)
				printf("%s\n", human_readable_time);
			else
				goto get_time_err;
		} else
			goto get_time_err;
	} else
		goto get_time_err;

	/* ok */
	return 0;

get_time_err:
	printf("Error getting time\n");
	return -1;
}
