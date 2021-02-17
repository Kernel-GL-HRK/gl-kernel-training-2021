#include <stdio.h>
#include <string.h>

enum errors {
	NO_ARGS = 1,
	TO_MUCH_ARGS,	/* 2 */
	WRONG_ARG,		/* 3 */
};

int main(int argc, char const *argv[]) {
	int ret = 0;

	if (argc < 2) {
		printf("Need one more argument\n");
		ret = NO_ARGS;
		goto end;
	}

	if (argc > 2) {
		printf("To match arguments\n");
		ret = TO_MUCH_ARGS;
		goto end;
	}

	char user_choise[2];

	if ((strcmp(argv[1], "r") == 0) ||
		(strcmp(argv[1], "p") == 0) ||
		(strcmp(argv[1], "s") == 0)) {

		strcpy(user_choise, argv[1]);
	} else {
		printf("Argument can be only 'r' 'p' 's'\n");
		ret = WRONG_ARG;
		goto end;
	}

	printf("Make test\n");
end:
	return ret;
}
