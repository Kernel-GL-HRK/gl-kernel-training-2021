// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <time.h>
#include "scissors.h"

int char_to_element_index(char c)
{
	int ans = UNKNOWN;

	switch (c) {
	case 'r':
		ans = ROCK;
		break;
	case 'p':
		ans = PAPER;
		break;
	case 's':
		ans = SCISSORS;
		break;
	}
	return ans;
}

void init_scissors(void)
{
	srand(time(NULL));
}

int play_scissors(void)
{
	return (rand()%3);
}
