// SPDX-License-Identifier: GPL-2.0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum result {ERROR = -1, TIE, USR_LOSE, USR_WIN};

enum result play(void)
{
	enum optn {ROCK, PAPER, SCISSORS};
	enum optn usr_optn;
	enum optn mchn_optn = rand()%3;
	char usr_optn_input;
	enum result res;

	fflush(stdin);
	printf("Please choose: 0 (rock) or 1 (paper) or 2 (scissors)\n");
	scanf(" %c", &usr_optn_input);

	if (usr_optn_input != '0' && usr_optn_input != '1'
			&& usr_optn_input != '2') {
		printf("Wrong input! Error\n");
		return -1;
	}

	usr_optn =(int) usr_optn_input - '0';

	printf("You chose %d, I chose %d\n", usr_optn, mchn_optn);

	if (usr_optn == mchn_optn)
		return res = TIE;

	switch (mchn_optn) {
	case ROCK:
		if (usr_optn == PAPER)
			res = USR_WIN;
		else
			res = USR_LOSE;
		break;
	case PAPER:
		if (usr_optn == ROCK)
			res = USR_LOSE;
		else
			res = USR_WIN;
		break;
	case SCISSORS:
		if (usr_optn == ROCK)
			res = USR_WIN;
		else
			res = USR_LOSE;
		break;
	default:
		printf("Error generating machine option. Exiting...\n");
		res = ERROR;
	}

	return res;
}

int invite_to_play(void)
{
	fflush(stdin);
	char usr_key = ' ';

	printf("Do you want to play the game? y or n\n");
	scanf(" %c", &usr_key);

	switch (usr_key) {
	case 'y':
		/* fall through */
	case 'Y':
		return 1;
	default:
		printf("Any option except 'y' is interpreted as 'no'\n");
		break;
	}

	return 0;
}

int main(void)
{
	srand(time(NULL));
	int dcsn = invite_to_play();

	if (dcsn == 1) {
		enum result res  = play();

		if (res == ERROR)
			return -1;
		else if (res == TIE)
			printf("It's a tie\n");
		else if (res == USR_LOSE)
			printf("You lost =(\n");
		else
			printf("You won!\n");
	} else {
		printf("Game over\n");
	}

	return 0;
}

