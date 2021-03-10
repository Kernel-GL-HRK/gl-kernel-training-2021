// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <time.h>
#include "scissors.h"

/*
 *	Table indicates user's lost or win
 *	The row index corresponds to user's choise
 *	The column index corresponds to comp's choice
 */

static const int game_table[3][3] = {
/*		rock	paper	scissors	*/
/* rock*/	DRAW,	LOST,	WINN,
/* paper*/	WINN,	DRAW,	LOST,
/* sciss*/	LOST,	WINN,	DRAW
};

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

int get_game_result(int user, int comp)
{
	return game_table[user][comp];
}
