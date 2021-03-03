#include <stdio.h>
#include <time.h>

const int T_BUF_SIZE = 50;

int main(void)
{
	char tbuff[T_BUF_SIZE];
	time_t t = time(NULL);
	time_t t_begin = 0;
	struct tm *t_gmtime = gmtime(&t);
	struct timespec tp = {0};

	clock_gettime(0, &tp);

	printf("Clock_gettime() - "
		"Seconds: %li Nanoseconds: %li\n", tp.tv_sec, tp.tv_nsec);
	printf("Curent time in seconds time(): %li\n", t);
	printf("Unix epoch: %s", ctime(&t_begin));
	printf("Current local time: %s\n", ctime(&t));

	strftime(tbuff, T_BUF_SIZE, "%B %Y\n%d %A\n%-I:%M:%S %p", t_gmtime);
	printf("Current gm time:\n%s\n", tbuff);

	return 0;
}
