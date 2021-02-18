#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//TODO: reformat code;
//      ask if wanna play again;
//      convert everything either to digits or to numbers

/* 0 tie, 1 machine wind, 2 user wins */
int play(void) {
    fflush(stdin);
    /* 0 == rock, 1 == paper, 2 == scissors */
    int mchn_optn = rand()%3;
    char usr_optn;
    printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
    scanf(" %c", &usr_optn);
    if (usr_optn != 'r' && usr_optn != 'p' && usr_optn != 's') {
        printf("Wrong input! Error\n");
        return -1;
    }
    printf("You chose %c, I chose %d\n", usr_optn, mchn_optn);
    switch (mchn_optn) {
        case 0:
            if (usr_optn == 'r')
                return 0;
            else if (usr_optn == 'p')
                return 1;
            else
                return 2;
            break;
        case 1:
            if (usr_optn == 'r')
                return 1;
            else if (usr_optn == 'p')
                return 0;
            else
                return 2;
            break;
        case 2:
            if (usr_optn == 'r')
                return 2;
            else if (usr_optn == 'p')
                return 1;
            else
                return 0;
            break;
        default:
            printf("Error generating machine option. Exiting...\n");
            return -1;
            break;
    }
    return 0;
}

int invite_to_play(void) {
    fflush(stdin);
    char usr_key = 'y';
    printf("Do you want to play the game? y or n \n");
    scanf(" %c", &usr_key);
    switch (usr_key) {
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
    srand(time(NULL));
    int dcsn = invite_to_play();
    if (dcsn == 1) {
        int result = play();
        if (result == -1)
            return result;
        else if (result == 0)
            printf("It's a tie\n");
        else if (result == 1)
            printf("You lost =( \n");
        else
            printf("You won! \n");
    }
    else if (dcsn == -1)
        printf("Unknown option. Error \n");
    else
        printf("Game over \n");

    return 0;
}
