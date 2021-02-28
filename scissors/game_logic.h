/* SPDX-License-Identifier: GPL-2.0+ */
/**
 * @file		game_logic.h
 * @brief		game_logic
 * @author		Leonid Chebenko
 * @version		1.0
 */

#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#define USER_WIN		1
#define COMPUTER_WIN		2
#define DRAW			0
#define ROCK			1
#define SCISSOR			2
#define PAPER			3

int game_logic(int player, int computer);

#endif /* __GAME_LOGIC_H__ */
