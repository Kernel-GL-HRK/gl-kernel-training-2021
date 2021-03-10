TARGET ?= main
SRC := main.c
CC = /opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CFLAGS = -O2 -Wall -Wpedantic
REMDIR ?= /srv/nfs/busybox/home
.PHONY: all clean tidy deploy

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
deploy:
	sudo cp $(TARGET) $(REMDIR)

