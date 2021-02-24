#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum figure_num_t {
        PAPER = 0,
        ROCK,
        SCISSORS,
        MAX_FIGURE_NUM
};

static const char FIGURE_SYM[3] = { 'p', 'r', 's' };

static const char *FIGURE_FULL_NAME[] = { "paper", "rock", "scissors" };

int main(void)
{
        enum figure_num_t player_num;
        enum figure_num_t bot_num;
        char bot_fig;
        char player_fig;
        bool player_step_flag;

        printf("Press \"Ctrl + C\" for exit\n");

        while(1) {

                bot_num = rand()%3;

                bot_fig = FIGURE_SYM[bot_num];

                do {
                        player_step_flag = false;

                        printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
                        scanf("\n%c", &player_fig);

                        if (player_fig == FIGURE_SYM[PAPER])
                                player_num = PAPER;
                        else if (player_fig == FIGURE_SYM[ROCK])
                                player_num = ROCK;
                        else if (player_fig == FIGURE_SYM[SCISSORS])
                                player_num = SCISSORS;
                        else
                                player_step_flag = true;

                } while (player_step_flag);

                printf("Player: %s\tBot: %s\n", FIGURE_FULL_NAME[player_num], FIGURE_FULL_NAME[bot_num]);


                if (player_num == bot_num)
                        printf("Draw:|\n");
                else if ((abs(player_num - bot_num) < (MAX_FIGURE_NUM - 1) && player_num < bot_num) 
                        || (SCISSORS == player_num && abs(player_num - bot_num) >= (MAX_FIGURE_NUM - 1)))

                        printf("Player won:)\n");
                else
                        printf("Bot won:(\n");    
        }

	return 0;
}
