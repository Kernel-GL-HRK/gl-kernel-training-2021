CC=arm-linux-gnueabihf-gcc
CFLAGS=-O2

TARGET=time
SOURCE=time
OBJ:=$(addsuffix .o, $(SOURCE))

export PATH := /opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin:$(PATH)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(TARGET)
%.o: %.c
	$(CC) $(CFLAGS) -c $<
clean:
	rm -rf *.o $(TARGET)
deploy:
	cp ./$(TARGET) /srv/nfs/busybox/
