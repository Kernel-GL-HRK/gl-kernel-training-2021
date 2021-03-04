NAME = absolute_time

SRC = main.c

CFLAGS = -Wall

OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/,$(SRC:.c=.o))

CC=${CROSS_COMPILE}gcc

.PHONY = all clean re send

all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(NAME)

$(addprefix $(OBJ_DIR),/%.o): %.c
	$(CC) $(CFLAGS) $(DDFLAGS) -c $^ -o $@

clean:
	rm -rf $(OBJ_DIR) $(NAME)

re:
	make clean
	make all

send:
	sudo cp $(NAME) /srv/nfs/busybox/tmp

$(OBJ_DIR):
	mkdir -p $@
