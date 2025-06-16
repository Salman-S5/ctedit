CC=gcc
CFLAGS=-Iinclude -Wall -lncurses
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
TARGET=build/editor

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf build/*

run:
	./build/editor
