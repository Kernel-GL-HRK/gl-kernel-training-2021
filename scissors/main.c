#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

enum {
	NONE	= -1,
	ROCK	= 0,
	SCISSORS= 1,
	PAPER	= 2,
	OPTS_MAX
}options;

const int rules_martix[OPTS_MAX][OPTS_MAX] =
{
	{NONE,	ROCK, PAPER},
	{ROCK,  NONE, SCISSORS},
	{PAPER, SCISSORS, NONE}
};

const char* choise_name[OPTS_MAX] =
{
	[ROCK] = "rock",
	[SCISSORS] = "scissors",
	[PAPER]	= "paper"
};

const char* id2choise(int optionId)
{
	if ((PAPER < optionId) || (NONE >= optionId))
		return NULL;
	else
		return choise_name[optionId];
}

int choise2id(char choiseChar)
{
	int indx = 0;

	while (indx < OPTS_MAX)
	{
		if (choise_name[indx][0] == choiseChar)
			return indx;
		else
			indx++;
	}
	return NONE;
}

int main()
{
	int pc_choise = NONE;
	int human_choise = NONE;

	printf("Please choose: rock(r) - paper(p) - scissors(s)\n");

	srand(time(NULL));
	pc_choise = rand() % 2;

	char temp;
	scanf("%c",&temp);
	human_choise = choise2id(temp);

	if ((PAPER < human_choise) || (NONE >= human_choise))
	{
		printf("Bad input!\n");
		return (1);
	}

	printf("You choose %s, I choose %s\n", id2choise(human_choise), id2choise(pc_choise));

	if (NONE == rules_martix[pc_choise][human_choise])
	{
		printf("Draw. \n");
	}
	else if (pc_choise == rules_martix[pc_choise][human_choise])
	{
		printf("I win: %s beats %s \n", id2choise(pc_choise), id2choise(human_choise));
	}
	else if (human_choise == rules_martix[pc_choise][human_choise])
	{
		printf("You win: %s beats %s \n", id2choise(human_choise), id2choise(pc_choise));
	}
	return (0);
}

