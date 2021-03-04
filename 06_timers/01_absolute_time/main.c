#include <stdio.h>
#include <time.h>

#define YEARS_SINCE 1900

const char *day_of_week[] = {"Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday", "Sanday"};
const char *months[] = {"January", "February", "March", "April", "May",
	"June", "July", "August", "September", "October", "November",
	"December" };

int main(void) {
	int ret;
	struct tm *ptm;
	struct timespec spec;

	ret = clock_gettime(CLOCK_REALTIME, &spec);

	if (ret == -1) {
		perror("The clock_gettime() function failed");
		return -1;
	}

	ptm = localtime(&spec.tv_sec);

	if (ptm == NULL) {
		perror("The localtime() function failed");
		return -1;
	}

	printf("%s, %02d %s %d %02d:%02d:%02d.%09ld\n", day_of_week[ptm->tm_wday],
		ptm->tm_mday, months[ptm->tm_mon], YEARS_SINCE + ptm->tm_year,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec, spec.tv_nsec);

	return 0;
}
