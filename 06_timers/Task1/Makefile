
KDIR ?= /home/$(USER)/repos/linux-stable

ch_style:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f main.c

build:
	gcc main.c -o main.o -Wall