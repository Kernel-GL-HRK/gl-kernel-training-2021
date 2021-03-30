#------------------------------------------------------------------------------
# ------------------------YOU CAN(and NEED) TO CHANGE IT-----------------------
#------------------------------------------------------------------------------
TARGET = usermem
#------------------------------------------------------------------------------
	
#you need to contain all you *.h files in dir "inc" IT'S A RULE
INC_DIR = inc
#flags for compiler
WFLAGS = -Wall -Werror -Wpedantic
#level of optimisation
OPTIM = -O2
#standart
STD = -std=gnu11
#compiler
CC = gcc
#linker's flags
LDFLAGS =

#------------------------------------------------------------------------------
src_files := $(wildcard *.c)
obj_files := $(src_files:.c=.o)
asm_files := $(src_files:.c=.s)

.PHONY: asm build clean tidy

#------------------------------------------------------------------------------
%.o: %.c
	$(CC) $(WFLAGS) $(OPTIM) $(STD) -I $(INC_DIR) -c $< -o $@

#------------------------------------------------------------------------------
%.s: %.c
	$(CC) $(WFLAGS) $(OPTIM) $(STD) -I $(INC_DIR) -fverbose-asm -S $< -o $@

#------------------------------------------------------------------------------
asm: $(asm_files)

#------------------------------------------------------------------------------
build: $(obj_files)
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

#------------------------------------------------------------------------------
clean:
	rm -f $(obj_files)
	echo "Only <TARGET> left here!:)"

#------------------------------------------------------------------------------
tidy: clean
	rm -i $(TARGET)
	echo "Your directory is tidy:)"
