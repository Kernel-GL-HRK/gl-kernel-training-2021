// SPDX-License-Identifier: GPL-2.0+
/**
 * @file		game_logic.c
 * @brief		game_logic
 * @author		Leonid Chebenko
 * @version		1.0
 */

#include "game_logic.h"

int game_logic(int player, int computer)
{
	int result;
	int game_continue;

	if (player == ROCK && computer == PAPER)
		result = COMPUTER_WIN;

	else if (player == ROCK && computer == SCISSOR)
		result = USER_WIN;

	else if (player == PAPER && computer == ROCK)
		result = USER_WIN;

	else if (player == PAPER && computer == SCISSOR)
		result = COMPUTER_WIN;

	else if (player == SCISSOR && computer == ROCK)
		result = COMPUTER_WIN;

	else if (player == SCISSOR && computer == PAPER)
		result = USER_WIN;

	else
		result = DRAW;

	return result;
}
