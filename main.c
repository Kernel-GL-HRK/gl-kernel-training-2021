#include <stdio.h>
#include <stdlib.h>


int invite_to_play(void) {
    char usr_key = 'y';
    printf("Do you want to play the game? y or n \n");
    scanf("%c", &usr_key);
    switch(usr_key)
    {
        case 'n': case 'N':
            return 0;
            break;
        case 'y': case 'Y':
            return 1;
            break;
        default:
            return -1;
            break;
    }
    return 0;
}

int main(void) {
    int dcsn = invite_to_play();
    if (dcsn == 1) {
       //play
    }
    else if (dcsn == -1) {
        printf("Unknown option. Error \n");
    }
    else {
        printf("Game over \n");
    }
    return 0;
}
