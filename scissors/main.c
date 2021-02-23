#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

enum {
	NONE	 = -1,
	ROCK	 = 0,
	SCISSORS = 1,
	PAPER	 = 2,
	OPTS_MAX
};

static const int rules_martix[OPTS_MAX][OPTS_MAX] = {
	{NONE,	ROCK, PAPER},
	{ROCK,  NONE, SCISSORS},
	{PAPER, SCISSORS, NONE}
};

static const char *uman_choise_name[OPTS_MAX] = {
	[ROCK] = "rock",
	[SCISSORS] = "scissors",
	[PAPER]	= "paper"
};

static int choise2id(char choise_char)
{
	int indx = 0;

	while (indx < OPTS_MAX)	{
		if (uman_choise_name[indx][0] != choise_char)
			indx++;
		else
			return indx;
	}
	return NONE;
}

int main(void)
{
	int pc_choise = NONE;
	int human_choise = NONE;

	printf("Please choose: rock(r) - paper(p) - scissors(s)\n");

	srand(time(NULL));
	pc_choise = rand() % OPTS_MAX;

	char temp;

	scanf("%c", &temp);
	human_choise = choise2id(temp);

	if ((human_choise > PAPER) || (human_choise <= NONE)) {
		printf("Bad input!\n");
		return 1;
	}

	printf("You choose %s, I choose %s.\n", uman_choise_name[human_choise], uman_choise_name[pc_choise]);

	if (rules_martix[pc_choise][human_choise] == NONE)
		printf("Draw.\n");
	else if (pc_choise == rules_martix[pc_choise][human_choise])
		printf("I win: %s beats %s.\n", uman_choise_name[pc_choise], uman_choise_name[human_choise]);
	else if (human_choise == rules_martix[pc_choise][human_choise])
		printf("You win: %s beats %s.\n", uman_choise_name[human_choise], uman_choise_name[pc_choise]);

	return 0;
}
