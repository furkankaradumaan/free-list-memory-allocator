CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic -Werror

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.o, ovj/%.o, $(SRCS))

TARGET = main
TEST = tests/test

all: $(TARGET) $(TEST)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TEST): tests/allocate_test.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(TARGET) $(TEST) $(OBJS)
