#include <stdio.h>

#include "all_tests.h"

int main(int argc, char **argv) {
  int failure_count = 0;
  failure_count += counter_variable_tests();
  failure_count += sample_variable_tests();
  failure_count += hash_table_tests();
  failure_count += json_helpers_tests();
  failure_count += input_parser_tests();
  failure_count += executor_tests();
  return failure_count;
}
