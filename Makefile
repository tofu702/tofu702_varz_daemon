CC=gcc
CFLAGS=-Wall -std=c99 -g
SOURCES=counter_variable.c time_utils.c sample_variable.c
TEST_SOURCES=$(SOURCES) counter_variable_test.c


build: $(TEST_SOURCES)
	$(CC) -o test $(TEST_SOURCES) $(CFLAGS)

test: build
	./test

