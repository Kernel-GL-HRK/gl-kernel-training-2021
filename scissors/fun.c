#include "fun.h"

#include <stdlib.h>
#include <time.h>

#define MAX_GEN_VALUE 3

char gen_pc_choice()
{
	srand(time(NULL));
	const char answ_arr[MAX_GEN_VALUE] = {'r', 'p', 's'};
	return answ_arr[rand() % MAX_GEN_VALUE];
}
