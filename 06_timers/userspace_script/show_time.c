#include <stdio.h>
#include <time.h>

int main(void)
{
	struct timespec now = {0};
	struct tm *hrt = {0};		// human readable time

	clock_gettime(0, &now);
	hrt = localtime(&now.tv_sec);

	printf("It is %d:%d:%d %d-%d-%d\n",
			hrt->tm_hour, hrt->tm_min, hrt->tm_sec,
			hrt->tm_year + 1900,
			hrt->tm_mon + 1,
			hrt->tm_mday);

	return 0;
}
