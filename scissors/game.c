#include <stdio.h>

const char* ParseChoice(char choice)
{
    switch (choice) 
    {
        case 'p':
            return "paper";
        case 'r':
            return "rock";
        case 's':
            return "scissors";
    }
}

int main(int argc, char *argv[])
{
    char player1;
    char player2;

    printf("Welcome to game!\n");
    printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	
    if (argc != 3) {
        printf("Please enter correct parameter\n");
        printf("Run: ./scissors Player1_choice, Player2_choice: p, r or s\n");
        return -1;
    }
    
    sscanf(argv[1], "%c", &player1);
    sscanf(argv[2], "%c", &player2);
    
    if ((player1 != 'p') && (player1 != 'r') && (player1 != 's')) {
        printf("Player1, please, enter correct choice!\n");
        return -1;
    }

    if ((player2 != 'p') && (player2 != 'r') && (player2 != 's')) {
        printf("Player2, please, enter correct choice!\n");
        return -1;
    }

    printf("Player1 choose: %s, Player2 choose: %s\n", 
            ParseChoice(player1), ParseChoice(player2));

    if (player1 == player2) {
        printf("Draw!\n");
    } else if ((player1 == 'p') && (player2 == 'r')) {
        printf("Player1 win: paper beats rock\n");
    } else if ((player1 == 'p') && (player2 == 's')) {
        printf("Player2 win: scissors beats paper\n");
    } else if ((player1 == 'r') && (player2 == 'p')) {
        printf("Player2 win: paper beats rock\n");
    } else if ((player1 == 'r') && (player2 == 's')) {
        printf("Player1 win: rock beats scissors\n");
    } else if ((player1 == 's') && (player2 == 'p')) {
        printf("Player1 win: scissors beats paper\n");
    } else if ((player1 == 's') && (player2 == 'r')) {
        printf("Player2 win: rock beats scissors\n");
    } 
    
    return 0;
}
