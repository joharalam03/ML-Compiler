CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC = src/ml_translator.c
TARGET = ml_translator

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) output ml-*.c *.o *.out
