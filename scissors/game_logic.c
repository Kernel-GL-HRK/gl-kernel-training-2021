/**
 * @file       game_logic.c
 * @brief      game_logic
 * 
 * @author     Leonid Chebenko
 * @version    1.0
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "game_logic.h"

extern int computer;
extern int player;
extern void start();

void game_logic()
{
        int game_continue;

        computer = rand()% 3+1;

        printf("Rock...");
        sleep(1);
        printf("Scissors...");
        sleep(1);
        printf("Paper...");
        sleep(1);
        printf("One...");
        sleep(1);
        printf("Two...");
        sleep(1);
        printf("Three...");

        short result = (computer - player) % 3;

        switch (result < 0 ? result + 3 : result) {
        case 0:
                printf("Draw!\n");
                break;

        case 1:
                printf("You won, congrands !\n");
                break;

        case 2:
                printf("Computer won!\n");
                break;
        }

        printf("Wanna play more? [1] - Yes, [2] - No\n");

        scanf("%d", &game_continue);

        switch (game_continue) {
        case 1:
                start();
                break;

        case 2:
                printf("You gave up on the game");
                exit(0);
                break;

        default :
                printf("Eror 509");
                break;
        }
}