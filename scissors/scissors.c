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
	TO_MUCH_ARGS,	/* 2 */
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
		fprintf(stderr, "To match arguments\n");
		ret = TO_MUCH_ARGS;
		goto end;
	}

	char user_choise[2];

	if ((strcmp(argv[1], "r") == 0) ||
		(strcmp(argv[1], "p") == 0) ||
		(strcmp(argv[1], "s") == 0)) {

		strcpy(user_choise, argv[1]);
	} else {
		fprintf(stderr, "Argument can be only 'r' 'p' 's'\n");
		ret = WRONG_ARG;
		goto end;
	}

	char game_elements[3][2] = {"r", "p", "s"};
	srand(time(NULL));
	unsigned short pc_choise = rand() % 3; 

	if (!strcmp(game_elements[pc_choise], user_choise)) {
		END_GAME_MESSAGE(user_choise, game_elements[pc_choise], "DRAW!");
	} else if ((!strcmp(game_elements[pc_choise], "r") && !strcmp(user_choise, "p")) ||
				(!strcmp(game_elements[pc_choise], "p") && !strcmp(user_choise, "s")) ||
				(!strcmp(game_elements[pc_choise], "s") && !strcmp(user_choise, "r"))) {
		END_GAME_MESSAGE(user_choise, game_elements[pc_choise], "YOU WIN!");
	} else {
		END_GAME_MESSAGE(user_choise, game_elements[pc_choise], "PC WIN!");
	}

end:
	return ret;
}

