CC=gcc
CFLAGS=-Wall -std=c99 -g
SOURCES=counter_variable.c time_utils.c sample_variable.c hash_table.c
TEST_SOURCES=$(SOURCES) counter_variable_test.c sample_variable_test.c hash_table_test.c all_tests.c
DAEMON_SOURCES=$(SOURCES) daemon.c

build: build_test build_daemon

build_daemon: $(DAEMON_SOURCES)
	$(CC) -o daemon $(DAEMON_SOURCES) $(CFLAGS)

build_test: $(TEST_SOURCES)
	$(CC) -o test $(TEST_SOURCES) $(CFLAGS)

test: build_test
	./test
