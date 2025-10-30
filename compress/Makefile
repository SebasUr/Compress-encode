CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2
LDFLAGS =

SRC = main.c huffman.c bitwriter.c bitreader.c compress.c
OBJ = $(SRC:.c=.o)
BIN = huffman

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c huffman.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)
