#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define BUFFER_LENGTH   128

struct parts_of_nsec {
    uint16_t msec;
    uint16_t usec;
    uint16_t nsec;
};

void divide_nsec(int64_t t, struct parts_of_nsec *divided)
{
    divided->msec = t/1000000;
    divided->usec = (t%1000000)/1000;
    divided->nsec = t%1000;
}

int main() 
{   
    struct timespec timer;

    clock_gettime(CLOCK_REALTIME, &timer);

    int64_t time_in_sec = (timer.tv_sec);

    struct parts_of_nsec div_tim;

    struct tm *local_time = localtime(&(time_in_sec));

    divide_nsec(timer.tv_nsec, &div_tim);

    printf("Current time is: %d:%d:%d:%d:%d:%d\n", local_time->tm_hour, local_time->tm_min,
                                    local_time->tm_sec, div_tim.msec, div_tim.usec, 
                                    div_tim.nsec);

    return 0;
}
