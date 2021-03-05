#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

char *define_computer(int num, char *comp_var)
{

	if (num == 0)
		strcpy(comp_var, "p\0");
	else if (num == 1)
		strcpy(comp_var, "s\0");
	else if (num == 2)
		strcpy(comp_var, "r\0");
	return (comp_var);

}

void print_result(char *human_var, char *computer_var, int win)
{
	if (win == 1)
		printf("\nYeap. You win.\n");
	else
		printf("\nOps. You lose.\n");
	printf("Because you pick %s and  computer pick %s\n", human_var, computer_var);
		
}

void game_start(char *human_var)
{
	srand(time(NULL));
	int r = rand() % 2;
	char computer_var[2];
	strcpy(computer_var,define_computer(r, computer_var));

	if (strcmp(computer_var, human_var) == 0)
		printf("\nHmm...Nice try, but it`s draw.You both pick %s\n", human_var);
	else if (strcmp(computer_var, "p") == 0 && \
		strcmp(human_var, "s") == 0)
		print_result(human_var, computer_var, 1);
	else if (strcmp(computer_var, "r") == 0 && \
		strcmp(human_var, "p") == 0)
		print_result(human_var, computer_var, 1);
	else if (strcmp(computer_var, "s") == 0 && \
		strcmp(human_var, "r") == 0)
		print_result(human_var, computer_var, 1);
	else if (strcmp(computer_var, "s") == 0 && \
		strcmp(human_var, "p") == 0)
		print_result(human_var, computer_var, 0);
	else if (strcmp(computer_var, "p") == 0 && \
		strcmp(human_var, "r") == 0)
		print_result(human_var, computer_var, 0);
	else if (strcmp(computer_var, "r") == 0 && \
		strcmp(human_var, "s") == 0)
		print_result(human_var, computer_var, 0);
	else 
		printf("\nHmm. I dont know this symbol.Please try again.\n");

}

int main(int argc, char **argv)
{
	if (argc == 1)
	        puts("Please choose: rock (r) - paper (p) - scissors (s)");
	else if (argc == 2)
		game_start(argv[1]);
	return (0);
}

