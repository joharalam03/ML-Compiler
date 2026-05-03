CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
LDLIBS  = -lm
SRC     = src/ml_translator.c
TARGET  = ml_translator

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

test: $(TARGET)
	@bash tests/run_tests.sh

clean:
	rm -f $(TARGET) ml-*.c *.o *.out