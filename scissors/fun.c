#include "fun.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define MAX_GEN_VALUE 3

static void print_full_name(const char name);

const char gen_pc_choice()
{
	srand(time(NULL));
	const char answ_arr[MAX_GEN_VALUE] = {'r', 'p', 's'};
	return answ_arr[rand() % MAX_GEN_VALUE];
}

void print_choices(const char h, const char pc)
{
	printf("You choose ");
	print_full_name(h);
	printf(", I choose ");
	print_full_name(pc);
	printf("\n");
}

void print_game_result(const char h, const char pc)
{
	if(h == pc) printf("It's a draw!\n");
	else if(h == 'p' && pc == 's') printf("You lost: paper lose scissors\n");
	else if(h == 'p' && pc == 'r') printf("You win: paper beats rock\n");
	else if(h == 's' && pc == 'p') printf("You win: scissors beats paper\n");
	else if(h == 's' && pc == 'r') printf("You lost: scissors lose rock\n");
	else if(h == 'r' && pc == 'p') printf("You win: rock beats paper\n");
	else if(h == 'r' && pc == 's') printf("You lost: rock beats scissors\n");
}

static void print_full_name(const char name)
{
	if(name == 'r') printf("rock");
	else if(name == 'p') printf("paper");
	else if(name == 's') printf("scissors");
}
