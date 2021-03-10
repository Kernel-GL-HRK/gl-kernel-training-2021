#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//"-1" - draw
// "0" - fail
// "1" - win

const int game_table[3][3]={
	-1,   0,  1,
	 1,  -1,  0,
	 0,   1, -1
};

int static char_to_text(char c, char *res, int size)
{
	int ans = -1;
	switch(c) {
	case 'r':
		strncpy(res, "rock", size);
		ans=0;
		break;
	case 'p':
		strncpy(res, "paper", size);
		ans=1;
		break;
	case 's':
		strncpy(res, "scissors", size);
		ans=2;
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
		if (user==-1)
		       continue;
		int check = game_table[user][comp];
		switch (check) {
		case 0:
			printf("I win: %s beats %s\n", ChNames[comp], res);
			break;
		case 1:
			printf("You win: %s beats %s\n", res, ChNames[comp]);
			break;
		case -1:
			printf("It's draw\n");
		}
	}
}
