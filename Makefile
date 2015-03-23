OS := $(shell uname -s)
ifeq ($(OS),Linux)
	PLATFORM_FLAGS=-D _POSIX_C_SOURCE=200809L
else
	PLATFORM_FLAGS=
endif

CC=gcc
CFLAGS=-Wall -std=c99 -g $(PLATFORM_FLAGS)
BENCH_CFLAGS=$(CFLAGS) -O3
THIRDPARTY_SOURCES=sds/sds.c
SOURCES=$(THIRDPARTY_SOURCES) counter_variable.c time_utils.c sample_variable.c hash_table.c json_helpers.c input_parser.c random.c hash_funcs.c executor.c
TEST_SOURCES=$(SOURCES) counter_variable_test.c sample_variable_test.c executor_test.c hash_table_test.c json_helpers_test.c input_parser_test.c all_tests.c
BENCH_SOURCES=$(SOURCES) benchmark.c
TEST_FLAGS=-D VARZ_STUB
DAEMON_SOURCES=$(SOURCES) daemon.c

VALGRIND_FLAGS=--leak-check=full --show-leak-kinds=all

build: build_test build_daemon

build_daemon: $(DAEMON_SOURCES)
	$(CC) -o daemon $(DAEMON_SOURCES) $(CFLAGS)

build_bench: $(BENCH_SOURCES)
	$(CC) -o bench $(BENCH_SOURCES) $(BENCH_CFLAGS)

build_test: $(TEST_SOURCES)
	$(CC) $(TEST_FLAGS) -o test $(TEST_SOURCES) $(CFLAGS)

test: build_test
	./test

valgrind_test: build_test
	valgrind $(VALGRIND_FLAGS) --suppressions=valgrind_test.suppressions ./test

bench: build_bench
	./bench

valgrind_bench: build_bench
	valgrind $(VALGRIND_FLAGS) ./bench

os:
	echo "$(OS)" "$(PROCESSOR_ARCHITECTURE)"
