CC=gcc

TARGET=usr_mem_test
SOURCE=usr_mem_test

OBJ:=$(addsuffix .o, $(SOURCE))

.PHONY: clean tidy

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)
%.o: %.c
	$(CC) -c $<
clean:
	rm -f $(OBJ)

tidy:
	rm -i $(TARGET)
