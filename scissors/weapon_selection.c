/**
 * @file       weapon_selection.c
 * @brief      weapon_selection
 * 
 * @author     Leonid Chebenko
 * @version    1.0
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "weapon_selection.h"
#include "game_logic.h"

void weapon_selection()
{
        int player;

        printf("Choose your weapon\n");

        printf(" [1] rock\n [2] scissors\n [3] Paper\n");

        scanf("%d", &player); 

        switch (player) {
        case 1:
                printf("The rock have been seleted\n");
                break;

        case 2:
                printf("The scissors have been seleted\n");
                break;

        case 3:
                printf("The paper have been seleted\n");
                break;

        default:
                printf("Undefined, please select one more time\n");
                weapon_selection();
                break;
        }

        game_logic();
}