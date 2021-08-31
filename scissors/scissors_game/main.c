// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "scissors.h"

static const char * const Game_Elements[] = {
	"rock", "paper", "scissors", "unknown"};

int main(void)
{
	char input;
	int res;
	int comp_choise;

	init_scissors();
	do {
		printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
		scanf("%1s", &input);
		res = char_to_element_index(input);
		if (res == UNKNOWN) {
			printf("You've chosen something strange!\n");
			continue;
		}
		comp_choise = play_scissors();
		printf("You choose %s, I choose %s\n",
			Game_Elements[res], Game_Elements[comp_choise]);
		switch (get_game_result(res, comp_choise)) {
		case LOST:
			printf("I win: %s beats %s\n",
				Game_Elements[comp_choise], Game_Elements[res]);
			break;
		case WINN:
			printf("You win: %s beats %s\n",
				Game_Elements[res], Game_Elements[comp_choise]);
			break;
		case DRAW:
			printf("It's draw\n");
			break;
		}
	} while (1);
}

