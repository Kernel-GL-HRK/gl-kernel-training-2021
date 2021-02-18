#include <stdio.h>

#include "fun.h"

int main()
{
	const char pc_choice = gen_pc_choice();
   	char hum_choice = 0;
   	
   	printf(">Please choose: rock (r) - paper (p) - scissors (s).  Exit - (q)\n");
   		
   	do {
		printf(">");
		hum_choice = getchar();
		if(hum_choice == 'q') return 0;
	}while(hum_choice != 'r' && hum_choice != 'p' && hum_choice != 's');
	
	print_choices(hum_choice, pc_choice);
	
	print_game_result(hum_choice, pc_choice);
	
	return 0;
}
