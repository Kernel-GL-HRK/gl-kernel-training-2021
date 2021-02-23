#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

const char* choice[] = { "s", "p", "r"};
const char* choice_long[] = { "scissors", "paper", "rock"};

int get_random(int lower, int upper)
{
	int num = (rand() % (upper - lower + 1)) + lower;
}

int get_choice_index(const char* str)
{
	for(int i = 0; i < 3; ++i)
	{
		if(strcmp(str,choice[i])==0) return i;
	}

	return -1;
}

void print_winner(int pc_idx, int h_idx)
{
	if(pc_idx == h_idx) {
		//draw
		printf("Draw! %s is %s\n", choice_long[pc_idx], choice_long[h_idx]);
	} else if((pc_idx - h_idx) == -1) {
		//pc win
		printf("I win: %s beats %s\n", choice_long[pc_idx], choice_long[h_idx]);
	} else if((pc_idx - h_idx) == -2) {
		//human win
		printf("You win: %s beats %s\n", choice_long[h_idx], choice_long[pc_idx]);
	} else if((pc_idx - h_idx) == 1) {
		//human win
		printf("You win: %s beats %s\n", choice_long[h_idx], choice_long[pc_idx]);
	} else if((pc_idx - h_idx) == 2) {
		//pc win
		printf("I win: %s beats %s\n", choice_long[pc_idx], choice_long[h_idx]);
	}
}

int main(int argc, char** argv)
{
	char choose[20] = {0};

	srand(time(NULL));
	
	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	scanf("%s",choose);

	int pc_choice_idx = get_random(0,2);
	int human_choice_idx = get_choice_index((const char*)choose);

	if(human_choice_idx < 0) return -1;
	
	printf("You choose %s, I choose %s\n", choice_long[human_choice_idx], choice_long[pc_choice_idx]);
	print_winner(pc_choice_idx,human_choice_idx);
}
