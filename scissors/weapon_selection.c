/**
 * @file		weapon_selection.c
 * @brief		weapon_selection
 * @author		Leonid Chebenko
 * @version		1.0
 */

#include "weapon_selection.h"
#include "game_logic.h"
#include <stdio.h>

void print_weapon(int weapon)
{
	if (weapon == ROCK)
		printf("ROCK\n");

	else if (weapon == PAPER)
		printf("PAPER\n");

	else if (weapon == SCISSOR)
		printf("SCISSOR\n");
}

int weapon_selection(void)
{
	int choice;

	do {
		printf("Choose your weapon\n");
		printf(" [1] rock\n [2] scissors\n [3] Paper\n");
		scanf("%d", &choice);

		if (!(choice >= 1 && choice <= 3))
			printf("ERROR: Invalid input");
	} while (!(choice >= 1 && choice <= 3));

	return choice;
}
