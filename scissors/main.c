#include <stdio.h>
#include <stdlib.h>

#define INIT_PROGRAM 0
#define EXIT_PROGRAM 1

enum {
	INDEX_ROCK = 0,
	INDEX_PAPER,
	INDEX_SCISSORS,
	INDEX_QUIT_GAME,
	INDEX_MAX
};

enum {
	WIN_RESULT_0 = -2,
	WIN_RESULT_1 = 1
};

static const char game_symbols[] = "rpsq";
static const char *game_words[] = {"rock", "paper", "scissers"};

static int get_index(void)
{
	char player_index;
	int i, flag = 0;


	player_index = getchar();
	for (i = 0; i < sizeof(game_symbols) / sizeof(game_symbols[0]); i++) {
		if (player_index == game_symbols[i]) {
			flag = 1;
			break;
		}
	}

	if (!flag)
		return -1;

	if (getchar() != '\n')
		return -1;

	return i;
}

static void print_choice(void)
{
	printf("> Please choose: rock (r) - paper (p) - scissors (s) or (q) "
			"for quit\n> ");
}

static void print_warning(void)
{
	printf("\nWarning: you choose wrong weapon!\n\n");
}

int main(void)
{
	int player_index, bot_index, res, is_exit = INIT_PROGRAM;

	while (is_exit != EXIT_PROGRAM) {
		bot_index = rand() % INDEX_QUIT_GAME;
		print_choice();

		player_index = get_index();
		if (player_index == -1) {
			print_warning();
			continue;
		}

		switch(player_index) {
		case INDEX_ROCK:
		case INDEX_PAPER:
		case INDEX_SCISSORS:
			res = player_index - bot_index;
			printf("You choose %s, I choose %s, ",
					game_words[player_index],
					game_words[bot_index]);
			if (res == WIN_RESULT_0 || res == WIN_RESULT_1)
				printf("you win, %s beats %s!\n\n",
						game_words[player_index],
						game_words[bot_index]);
			else if (!res)
				printf("this is draw!\n\n");
			else
				printf("you lose, %s beats %s!\n\n",
						game_words[bot_index],
						game_words[player_index]);
			break;
		case INDEX_QUIT_GAME:
			printf("Exit game\n");
			is_exit = EXIT_PROGRAM;
			break;
		default:
			break;
		}
	}

	return 0;
}
