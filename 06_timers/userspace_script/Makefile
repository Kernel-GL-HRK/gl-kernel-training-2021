CC=arm-linux-gnueabihf-gcc
CFLAGS=-O2
SOURCES = show_time.c
EXECUTABLE = show_time
OBJECTS = $(SOURCES:.c=.o)
REMDIR ?= /srv/nfs/busybox/

export PATH := /opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin:$(PATH)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -c -o $@

deploy:
	sudo cp $(EXECUTABLE) $(REMDIR)

clean:
	rm -f *.o $(EXECUTABLE)
