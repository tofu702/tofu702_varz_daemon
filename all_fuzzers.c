#include "all_fuzzers.h"

#define NUM_ITERATIONS_PER_FUZZER 1000

int main(int argc, char **argv) {
  int failure_count = 0;
  failure_count += input_parser_fuzzer(NUM_ITERATIONS_PER_FUZZER);
  return failure_count;

}
