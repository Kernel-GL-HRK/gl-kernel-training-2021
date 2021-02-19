/**
 * @file       main.c
 * @brief      game rock scissors paper
 * 
 * @author     Leonid Chebenko
 * @version    1.0
 */

#include "game_logic.h"
#include "weapon_selection.h"

int player;
int computer;

int main();

void start();

int main()
{
        start();
        return 0;
}

void start()
{
        int start;
        printf("Do you wanna play... (1 - Yes, 2- Not)");
        scanf("%d", &start);

        switch(start) {
        case 1:
                weapon_selection();
                break;

        case 2:
                printf("You gave up on the game");
                exit(0);
                break;

        default:
                printf("EROR 509!");
                break;
        }
}
