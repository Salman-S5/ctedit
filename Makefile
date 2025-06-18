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
	@if [ -z "$$1" ]; then \
		$(TARGET); \
	else \
		$(TARGET) "$$1"; \
	fi
	
clean:
	rm -rf build/*
