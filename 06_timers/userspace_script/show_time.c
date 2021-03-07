#include <stdio.h>
#include <time.h>

int main(void)
{
	struct timespec now = {0};

	clock_gettime(0, &now);

	printf("Absolute time is %ld.%ld seconds.\n", now.tv_sec, now.tv_nsec);

	return 0;
}
