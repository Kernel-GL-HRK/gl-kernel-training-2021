#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

static char define_computer(int num)
{
	char comp_var;

	if (num == 0)
		comp_var = 'p';
	else if (num == 1)
		comp_var = 's';
	else if (num == 2)
		comp_var = 'r';
	return (comp_var);

}

static void print_result(char human_var, char computer_var, int win)
{
	if (win == 1)
		printf("\nYeap. You win.\n");
	else
		printf("\nOps. You lose.\n");
	printf("Because you pick %c and  computer pick %c\n", human_var, computer_var);
		
}

static void game_start(char human_var)
{
	srand(time(NULL));
	int r = rand() % 2;
	char computer_var;
	computer_var = define_computer(r);

	if (computer_var == human_var)
		printf("\nHmm...Nice try, but it`s draw.You both pick %c\n", human_var);
	else if (computer_var == 'p' && human_var == 's')
		print_result(human_var, computer_var, 1);
	else if (computer_var == 'r' && human_var == 'p')
		print_result(human_var, computer_var, 1);
	else if (computer_var == 's' && human_var == 'r')
		print_result(human_var, computer_var, 1);
	else if (computer_var == 's' && human_var == 'p')
		print_result(human_var, computer_var, 0);
	else if (computer_var == 'p' && human_var == 'r')
		print_result(human_var, computer_var, 0);
	else if (computer_var == 'r' && human_var == 's')
		print_result(human_var, computer_var, 0);
	else 
		printf("\nHmm. I dont know this symbol.Please try again.\n");

}

int main(int argc, char **argv)
{
	if (argc == 1)
	        puts("Please choose: rock (r) - paper (p) - scissors (s)");
	else if (argc == 2)
		game_start(argv[1][0]);
	return (0);
}

