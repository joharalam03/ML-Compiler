CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -lm
SRC = src/ml_translator.c
TARGET = ml_translator

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) output ml-*.c *.o *.out
