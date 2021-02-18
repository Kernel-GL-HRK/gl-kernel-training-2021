#include <stdio.h>
#include <stdlib.h>

enum {
	INDEX_ROCK = 0,
	INDEX_PAPER,
	INDEX_SCISSORS,
	INDEX_QUIT_GAME,
	INDEX_MAX
};

static const char game_symbols[] = "rpsq";

static int get_index(void)
{
	char player_index;
	int i, flag = 0;


	player_index = getchar();
	for (i = 0; i < INDEX_MAX; i++) {
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
	int player_index, bot_index;

	while (1) {
		bot_index = rand() % INDEX_QUIT_GAME;
		print_choice();

		player_index = get_index();
		if (player_index == -1) {
			print_warning();
		}

		if (player_index == INDEX_QUIT_GAME)
			break;
		
		printf("You choose %c, I choose %c\n",
				game_symbols[player_index],
				game_symbols[bot_index]);
	}

	return 0;
}
