#include <stdio.h>
#include <string.h>

void static char_to_text(char c, char *res, int size)
{
	switch(c) {
	case 'r':
		strncpy(res, "rock", size);
		break;
	case 'p':
		strncpy(res, "paper", size);
		break;
	case 's':
		strncpy(res, "scissors", size);
		break;
	default:
		strncpy(res, "unknown", size);
	}
}

int main(void)
{
	while(1){
		printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
		char input;
		char res [10];
		memset (res, 0, 10);
		scanf ("%1s", &input);
		char_to_text(input, res, sizeof(res)-1);
		printf("You choose %s and it's ok!\n", res);
	}
}
