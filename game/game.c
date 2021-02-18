#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum figure_num_t {
        PAPER_N = 0,
        ROCK_N,
        SCISSORS_N
};

enum figure_sym_t {
        PAPER_S =       'p',
        ROCK_S =        'r',
        SCISSORS_S =    's'
};

int main(void)
{
        char tmp = 0;
        uint8_t player;
        char bot_let = 0;
        int num_of_arg = 0;

        printf("Press \"Ctrl + C\" for exit\n");

        while(1) {

                do {
                        printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
                        num_of_arg = scanf("\n%c", &tmp);

                } while (tmp != PAPER_S && tmp != ROCK_S && tmp != SCISSORS_S);

                int bot = rand()%3;

                switch (tmp) {
                        case PAPER_S: player = PAPER_N; break;
                        case ROCK_S: player = ROCK_N; break;
                        case SCISSORS_S: player = SCISSORS_N; break;
                }

                switch (bot) {
                        case PAPER_N: bot_let = PAPER_S; break;
                        case ROCK_N: bot_let = ROCK_S; break;
                        case SCISSORS_N: bot_let = SCISSORS_S; break;
                }

                printf("Player: %c\tBot: %c\n", tmp, bot_let);

                if (player == bot) {
                        printf("NO ONE|\n");
                } else if (abs(player - bot) < 2) {
                        if (player < bot)
                                printf("Player won)\n");
                        else
                                printf("Bot won)\n");
                } else {
                        if(2 == player)
                                printf("Player won)\n");
                        else
                                printf("Bot won)\n");
                }

        }

	return 0;
}
