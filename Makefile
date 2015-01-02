CC=gcc
CFLAGS=-Wall -std=c99 -g
SOURCES=counter_variable.c time_utils.c sample_variable.c hash_table.c
TEST_SOURCES=$(SOURCES) counter_variable_test.c sample_variable_test.c hash_table_test.c all_tests.c

build: build_test

build_test: $(TEST_SOURCES)
	$(CC) -o test $(TEST_SOURCES) $(CFLAGS)

test: build_test
	./test
