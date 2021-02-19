#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void end_game_message(char usr_val, char pc_val, char *message) 
{
	printf("Your choise - %c. Computer choise - %c. %s\n", usr_val, pc_val, message);
}

enum errors {
	NO_ARGS = 1,
	TOO_MUCH_ARGS,	/* 2 */
	WRONG_ARG,		/* 3 */
};

int main(int argc, char const *argv[]) {
	int ret = 0;

	if (argc < 2) {
		fprintf(stderr, "Need one more argument\n");
		ret = NO_ARGS;
		goto end;
	}

	if (argc > 2) {
		fprintf(stderr, "Too much arguments\n");
		ret = TOO_MUCH_ARGS;
		goto end;
	}

	if (strlen(argv[1]) > 1) {
		fprintf(stderr, "Wrong argument\n");
		ret = WRONG_ARG;
		goto end;
	}

	char user_choice = *argv[1];
	if (!(user_choice == 'r' || user_choice == 'p' || user_choice == 's')) {
		fprintf(stderr, "Argument can be only 'r' 'p' 's'\n");
		ret = WRONG_ARG;
		goto end;
	}

	const char game_elements[3] = {'r', 'p', 's'};
	srand(time(NULL));
	unsigned char pc_choice = rand() % 3; 

	if (user_choice == game_elements[pc_choice]) {
		end_game_message(user_choice, game_elements[pc_choice], "DRAW!");
	} else if ((game_elements[pc_choice] == 'r' && user_choice == 'p') ||
				(game_elements[pc_choice] == 'p' &&  user_choice == 's') ||
				(game_elements[pc_choice] == 's' &&  user_choice == 'r')) {
		end_game_message(user_choice, game_elements[pc_choice], "YOU WINS!");
	} else {
		end_game_message(user_choice, game_elements[pc_choice], "PC WINS!");
	}

end:
	return ret;
}

