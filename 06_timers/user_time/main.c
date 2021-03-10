#include <stdio.h>
#include <time.h>

#define YEARS_START 1900

int main(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    printf("now: %d-%02d-%02d %02d:%02d:%02d.%09ld\n", tm.tm_year + YEARS_START,
                tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                tm.tm_min, tm.tm_sec, spec.tv_nsec);

    return 0;
}

