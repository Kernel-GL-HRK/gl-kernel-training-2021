CC = gcc
CFLAGS = -g
NAME = game
$(NAME):weapon_selection.c game_logic.c main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	-rm -f *.o
	rm -f $(NAME)
