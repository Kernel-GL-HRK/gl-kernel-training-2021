
CC ?= gcc
SOURCES = main.c
EXECUTABLE = memtest
OBJECTS = $(SOURCES:.c=.o)
KDIR ?= /lib/modules/`uname -r`/build
REMDIR ?= .

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $< -c -o $@

deploy:
	sudo cp $(EXECUTABLE) $(REMDIR)
	
chpatch:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f $(SOURCES) > checkpatch.log	

clean:
	rm -f *.o $(EXECUTABLE)
