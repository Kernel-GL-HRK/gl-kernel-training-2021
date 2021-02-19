TARGET ?= scissors
SRC := scissors.c
CC = gcc
CFLAGS = -O2 -Wall -Wpedantic

.PHONY: clean tidy

all: $(TARGET)

$(TARGET): $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# remove build artifacts
clean:
	@echo Cleaning up
	rm -f $(SRC:.c=.o)

# remove build artifacts & target
tidy: clean
	rm -f $(TARGET)

