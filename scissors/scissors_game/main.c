#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "scissors.h"

int static char_to_text(char c, char *res, int size)
{
	int ans = eU;
	switch(c) {
	case 'r':
		strncpy(res, "rock", size);
		ans=eR;
		break;
	case 'p':
		strncpy(res, "paper", size);
		ans=eP;
		break;
	case 's':
		strncpy(res, "scissors", size);
		ans=eS;
		break;
	default:
		strncpy(res, "unknown", size);
	}
	return ans;
}

int main(void)
{
	srand(time(NULL));
	while(1){
		printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
		char input;
		char res [10];
		memset (res, 0, 10);
		scanf ("%1s", &input);
		int user = -1;
		user = char_to_text(input, res, sizeof(res)-1);
		static char *ChNames[] = {
			"rock",
			"paper",
			"scissors"
		};
		int comp=rand()%3;
		printf("You choose %s, I choose %s\n", res, ChNames[comp]);
		if (user==eU)
		       continue;
		int check = game_table[user][comp];
		switch (check) {
		case eLOST:
			printf("I win: %s beats %s\n", ChNames[comp], res);
			break;
		case eWINN:
			printf("You win: %s beats %s\n", res, ChNames[comp]);
			break;
		case eDRAW:
			printf("It's draw\n");
		}
	}
}
