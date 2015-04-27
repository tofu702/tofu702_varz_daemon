#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "all_tests.h"
#include "input_parser.h"
#include "random.h"


static struct VARZOperationDescription VARZOPCmdParseWithoutLen(char *sample_input) {
  return VARZOpCmdParse(sample_input, strlen(sample_input));
}


static int test_counter_parser_with_legal_input() {
  char *sample_input = "MHTCOUNTERADD test_variable 1 2;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_MHT_COUNTER_ADD) {
    return 1;
  } else if(strcmp("test_variable", desc.variable_name) != 0) {
    return 1;
  } else if(desc.op_data.counter_add_op.time != 1) {
    return 1;
  } else if(desc.op_data.counter_add_op.amt != 2) {
    return 1;
  }
  return 0;
}


static int test_counter_sets_invalid_for_illegal_time_string() {
  char *sample_input = "MHTCOUNTERADD test_variable foobar 2;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_INVALID) {
    printf("Error test_counter_sets_invalid_for_illegal_time_string, op should be %d, got %d\n",
        VARZOP_INVALID, desc.op);
    return 1;
  }
  return 0;
}


static int test_counter_sets_invalid_for_illegal_value_string() {
  char *sample_input = "MHTCOUNTERADD test_variable 1 foobar;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_INVALID) {
    printf("Error test_counter_sets_invalid_for_illegal_value_string, op should be %d, got %d\n",
        VARZOP_INVALID, desc.op);
    return 1;
  }
  return 0;
}


static int test_sample_parser_with_legal_input() {
  uint64_t stub_vals[] = {0x00000001000000002, 0x0000000300000004};
  char *sample_input = "MHTSAMPLEADD s 3 4;";
  struct VARZOperationDescription desc;

  VARZRand64StubValues(stub_vals, 2);
  desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_MHT_SAMPLE_ADD) {
    return 1;
  } else if(strcmp("s", desc.variable_name) != 0) {
    return 1;
  } else if(desc.op_data.sampler_add_op.time != 3) {
    return 1;
  } else if(desc.op_data.sampler_add_op.value != 4) {
    return 1;
  } else if(desc.op_data.sampler_add_op.random_vals[0] != stub_vals[0]) {
    return 1;
  } else if(desc.op_data.sampler_add_op.random_vals[1] != stub_vals[1]) {
    return 1;
  }

  return 0;
}


static int test_sample_sets_invalid_for_illegal_time_string() {
  char *sample_input = "MHTSAMPLEADD test_variable 3 foobar;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_INVALID) {
    printf("Error test_sample_sets_invalid_for_illegal_value_string, op should be %d, got %d\n",
        VARZOP_INVALID, desc.op);
    return 1;
  }
  return 0;
}


static int test_sample_sets_invalid_for_illegal_value_string() {
  char *sample_input = "MHTSAMPLEADD test_variable foobar 4;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_INVALID) {
    printf("Error test_sample_sets_invalid_for_illegal_time_string, op should be %d, got %d\n",
        VARZOP_INVALID, desc.op);
    return 1;
  }
  return 0;
}


static int test_sets_invalid_for_obviously_bad_input() {
  char *sample_input = "NOT_A_REAL_OP;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_INVALID) {
    printf("Error test_sets_invalid_for_obviously_bad_input, op should be %d, got %d\n", 
        VARZOP_INVALID, desc.op);
    return 1;
  }

  return 0;
}

static int test_sets_invalid_for_bad_operation_name() {
  char *sample_input = "NOT_A_REAL_OP a 1 2;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_INVALID) {
    printf("Error test_sets_invalid_for_bad_operation_name, op should be %d, got %d\n",
        VARZOP_INVALID, desc.op);
    return 1;
  }

  return 0;
}

static int test_all_dump_json_operation() {
  char *sample_input = "ALLDUMPJSON;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_ALL_DUMP_JSON) {
    printf("ERROR test_all_dump_json_operation, op should be %d, got %d\n", VARZOP_ALL_DUMP_JSON,
        desc.op);
    return 1;
  }
  return 0;
}

static int test_all_list_json_operation() {
  char *sample_input = "ALLLISTJSON;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_ALL_LIST_JSON) {
    printf("ERROR test_all_list_json_operation, op should be %d, got %d\n", VARZOP_ALL_LIST_JSON,
        desc.op);
    return 1;
  }
  return 0;
}

static int test_all_flush_operation() {
  char *sample_input = "ALLFLUSH;";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_ALL_FLUSH) {
    printf("ERROR test_all_flush_operation, op should be %d, got %d\n", VARZOP_ALL_FLUSH, desc.op);
    return 1;
  }
  return 0;
}

static int test_fails_if_no_trailing_semicolon() {
  char *sample_input = "ALLFLUSH";
  struct VARZOperationDescription desc = VARZOPCmdParseWithoutLen(sample_input);

  if (desc.op != VARZOP_INVALID) {
    printf("ERROR test_invalid, op should be %d, got %d\n", VARZOP_ALL_FLUSH, desc.op);
    return 1;
  }
  return 0;

}


int input_parser_tests() {
  int failure_count = 0;

  failure_count += test_counter_parser_with_legal_input();
  failure_count += test_counter_sets_invalid_for_illegal_time_string();
  failure_count += test_counter_sets_invalid_for_illegal_value_string();
  failure_count += test_sample_parser_with_legal_input();
  failure_count += test_sample_sets_invalid_for_illegal_time_string();
  failure_count += test_sample_sets_invalid_for_illegal_value_string();
  failure_count += test_sets_invalid_for_obviously_bad_input();
  failure_count += test_sets_invalid_for_bad_operation_name();
  failure_count += test_all_dump_json_operation();
  failure_count += test_all_list_json_operation();
  failure_count += test_all_flush_operation();
  failure_count += test_fails_if_no_trailing_semicolon();

  return failure_count;
}
