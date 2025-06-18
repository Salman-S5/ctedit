CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lncurses

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = build/ctedit

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	$(TARGET) $(FILE)
	
clean:
	rm -rf build/*
