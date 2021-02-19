CC = gcc
CFLAGS = -g
NAME = game
$(NAME):weapon_selection.c game_logic.c main.c
#OBJ = weapon_selection.o game_logic.o main.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	-rm -f *.o
	rm -f $(NAME)
