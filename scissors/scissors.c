#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define END_GAME_MESSAGE(user, pc, msg)	\
	do { printf("Your choise - %s. Computer choise - %s. %s\n",	\
			user, pc, msg);	\
			} while(0)

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

	char user_choice[2];

	if ((strcmp(argv[1], "r") == 0) ||
		(strcmp(argv[1], "p") == 0) ||
		(strcmp(argv[1], "s") == 0)) {

		strcpy(user_choice, argv[1]);
	} else {
		fprintf(stderr, "Argument can be only 'r' 'p' 's'\n");
		ret = WRONG_ARG;
		goto end;
	}

	char game_elements[3][2] = {"r", "p", "s"};
	srand(time(NULL));
	unsigned short pc_choice = rand() % 3; 

	if (!strcmp(game_elements[pc_choice], user_choice)) {
		END_GAME_MESSAGE(user_choice, game_elements[pc_choice], "DRAW!");
	} else if ((!strcmp(game_elements[pc_choice], "r") && !strcmp(user_choice, "p")) ||
				(!strcmp(game_elements[pc_choice], "p") && !strcmp(user_choice, "s")) ||
				(!strcmp(game_elements[pc_choice], "s") && !strcmp(user_choice, "r"))) {
		END_GAME_MESSAGE(user_choice, game_elements[pc_choice], "YOU WINS!");
	} else {
		END_GAME_MESSAGE(user_choice, game_elements[pc_choice], "PC WINS!");
	}

end:
	return ret;
}

