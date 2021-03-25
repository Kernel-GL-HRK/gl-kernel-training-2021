// SPDX-License-Identifier: GPL-2.0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//TODO: reformat code;
//      ask if wanna play again;
//      convert everything either to digits or to numbers

/* 0 tie, 1 machine wins, 2 user wins */
int play(void)
{
	fflush(stdin);
	/* 0 == rock, 1 == paper, 2 == scissors */
	int mchn_optn = rand()%3;
	char usr_optn;

	printf("Please choose: 0 (rock) or 1 (paper) or 2 (scissors)\n");
	scanf(" %c", &usr_optn);

	if (usr_optn != '0' && usr_optn != '1' && usr_optn != '2') {
		printf("Wrong input! Error\n");
		return -1;
	}

	printf("You chose %c, I chose %d\n", usr_optn, mchn_optn);

	if (atoi(&usr_optn) == mchn_optn)
		return 0;

	switch (mchn_optn) {
	case 0:
		if (usr_optn == '1')
			return 2;
		else
			return 1;
		break;
	case 1:
		if (usr_optn == '0')
			return 1;
		else
			return 2;
		break;
	case 2:
		if (usr_optn == '0')
			return 2;
		else
			return 1;

		break;
	default:
		printf("Error generating machine option. Exiting...\n");
		return -1;
	}

	return 0;
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
		return 0;
	}

	return -1;
}

int main(void)
{
	srand(time(NULL));
	int dcsn = invite_to_play();

	if (dcsn == 1) {
		int result = play();

		if (result == -1)
			return result;
		else if (result == 0)
			printf("It's a tie\n");
		else if (result == 1)
			printf("You lost =(\n");
		else
			printf("You won!\n");
	} else if (dcsn == -1) {
		printf("Unknown option. Error\n");
	} else {
		printf("Game over\n");
	}

	return 0;
}

