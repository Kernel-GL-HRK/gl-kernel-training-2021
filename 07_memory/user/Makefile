CC=${CROSS_COMPILE}gcc
OBJ=memory_task
CFLAGS=-I.

%.o: %.c
	@$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ).o $(OBJ).o
	@echo "Userspace: building target $(OBJ)..."
	@$(CC) -o $(OBJ) $(OBJ).o

clean:
	@echo "Userspace: cleaning..."
	@rm -f *.o $(OBJ)
