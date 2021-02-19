/**
 * @file       main.c
 * @brief      game rock scissors paper
 * 
 * @author     Leonid Chebenko
 * @version    1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_logic.h"
#include "weapon_selection.h"

int main()
{
        int quit = 0;
        int result;
        srand(time(NULL));

        while (!quit)
        {
                int user_input = weapon_selection();

                int comp_choice = (rand() % 3) + 1;

                printf("Computer choice: ");
                print_weapon(comp_choice);

                printf("User choice: ");
                print_weapon(user_input);

                result = game_logic(user_input, comp_choice);
                if (result == DRAW)
                        printf("It's a DRAW\n");

                else if (result == USER_WIN)
                        printf("User Win\n");

                else if (result == COMPUTER_WIN)
                        printf("Computer Win\n");

                printf("If you wanna to continue type any integer, 0 to terminate\n");
                int response;

                scanf("%d", &response);
                if (response == 0)
                        quit = 1;
        }

        return 0;
}
