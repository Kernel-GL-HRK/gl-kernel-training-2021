#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    char player_choose;
    int computer_choose;
    char possible_chooses[3] = {'p','r','s'};

    printf("Please choose: rock(r) - paper(p) - scissors(s)\n");
    scanf("%c",&player_choose);
    srand(time(0));
    computer_choose = rand()%2;

    switch (player_choose)
    {
    case 'r':
        if (possible_chooses[computer_choose] == 's') {
            printf("You choose rock, I choose scissors \n");
            printf("You win: rock beats scissors \n");
        }
        else if (possible_chooses[computer_choose] == 'p') {
                printf("You choose rock, I choose paper \n");
                printf("I win: paper beats rock \n");
                }
        else {
            printf("The Draw! Try again! \n");
        }
        break;
    case 'p':
        if (possible_chooses[computer_choose] == 's') {
            printf("You choose paper, I choose scissors \n");
            printf("I win: scissors beats paper \n");
        }
        else if (possible_chooses[computer_choose] == 'r') {
                printf("You choose paper, I choose rock \n");
                printf("You win: paper beats rock \n");
                }
        else {
            printf("The Draw! Try again! \n");
        }
        break;
    case 's':
        if (possible_chooses[computer_choose] == 'p') {
            printf("You choose scissors, I choose paper \n");
            printf("You win: scissors beats paper \n");
        }
        else if (possible_chooses[computer_choose] == 'r') {
                printf("You choose scissors, I choose rock \n");
                printf("I win: rock beats scissors \n");
                }
        else {
            printf("The Draw! Try again! \n");
        }
        break;
    default:
        printf("Error! Unexpected symbol inputted!\n");
        break;
    }

    return 0;
}
