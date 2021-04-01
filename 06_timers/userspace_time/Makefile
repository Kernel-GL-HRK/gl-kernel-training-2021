CC=arm-linux-gnueabihf-gcc
CFLAGS=-O2
SOURCES = current_time.c
OBJECTS = $(SOURCES:.c=.o)
REMDIR ?= /srv/nfs/busybox/

export PATH := /opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin:$(PATH)

current_time: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -c -o $@
clean:
	rm -f *.o current_time