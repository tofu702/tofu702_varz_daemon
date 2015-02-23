CC=gcc
CFLAGS=-Wall -std=c99 -g
BENCH_CFLAGS=-O3 -std=c99
THIRDPARTY_SOURCES=sds/sds.c
SOURCES=$(THIRDPARTY_SOURCES) counter_variable.c time_utils.c sample_variable.c hash_table.c json_helpers.c input_parser.c random.c hash_funcs.c executor.c
TEST_SOURCES=$(SOURCES) counter_variable_test.c sample_variable_test.c executor_test.c hash_table_test.c json_helpers_test.c input_parser_test.c all_tests.c
BENCH_SOURCES=$(SOURCES) benchmark.c
TEST_FLAGS=-D VARZ_STUB
DAEMON_SOURCES=$(SOURCES) daemon.c

build: build_test build_daemon

build_daemon: $(DAEMON_SOURCES)
	$(CC) -o daemon $(DAEMON_SOURCES) $(CFLAGS)

build_bench: $(BENCH_SOURCES)
	$(CC) -o bench $(BENCH_SOURCES) $(BENCH_CFLAGS)

build_test: $(TEST_SOURCES)
	$(CC) $(TEST_FLAGS) -o test $(TEST_SOURCES) $(CFLAGS)

test: build_test
	./test

bench: build_bench
	./bench
