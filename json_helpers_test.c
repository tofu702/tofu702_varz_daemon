#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sds/sds.h"

#include "all_tests.h"
#include "json_helpers.h"
#include "time_utils.h"

static int test_ul_arr_repr() {
  sds repr_sds = sdsempty();
  unsigned long test_arr[] = {3,1,4,1,5,26};

  VARZJSONUnsignedLongArrToRepr(&repr_sds, test_arr, 6);
  if (strcmp("[3,1,4,1,5,26]", repr_sds)) { 
    printf("ERROR: test_ul_arr_repr was expecting '[3,1,4,1,5,26]', got %s\n", repr_sds);
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}


static int test_time_repr() {
  sds repr_sds = sdsempty();
  varz_time_t t = 13;
  
  VARZJSONTimeRepr(&repr_sds, t);
  if (strcmp("13", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_array_start() {
  sds repr_sds = sdsempty();
  VARZJSONArrayStart(&repr_sds);
  if (strcmp("[", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_array_next_item() {
  sds repr_sds = sdsempty();
  VARZJSONArrayNextItem(&repr_sds);
  if (strcmp(",", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_array_end() {
  sds repr_sds = sdsempty();
  VARZJSONArrayEnd(&repr_sds);
  if (strcmp("]", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_dict_start() {
  sds repr_sds = sdsempty();
  VARZJSONDictStart(&repr_sds);
  if (strcmp("{", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_dict_next_key() {
  sds repr_sds = sdsempty();

  VARZJSONDictNextKey(&repr_sds);
  if(strcmp(",", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_dict_end() {
  sds repr_sds = sdsempty();
  VARZJSONDictEnd(&repr_sds);
  if(strcmp("}", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_dict_with_normal_key() {
  sds repr_sds = sdsempty();
  VARZJSONDictKey(&repr_sds, "foobar");
  if(strcmp("\"foobar\":", repr_sds)) {
    printf("ERROR test_dict_with_normal_key, expected '\"foobar\":', got '%s'\n", repr_sds);
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_dict_with_quote_in_key() {
  sds repr_sds = sdsempty();
  VARZJSONDictKey(&repr_sds, "foo\"bar");
  if(strcmp("\"foo\\\"bar\":", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_dict_with_many_escape_chars() {
  sds repr_sds = sdsempty();
  VARZJSONDictKey(&repr_sds, "\t\b\"\f\r\n");
  if(strcmp("\"\\t\\b\\\"\\f\\r\\n\":", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;

}

static int test_string_repr() {
  sds repr_sds = sdsempty();
  VARZJSONStringRepr(&repr_sds, "foo\"bar");
  if(strcmp("\"foo\\\"bar\"", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

static int test_unsigned_long_repr() {
  sds repr_sds = sdsempty();
  VARZJSONUnsignedLongRepr(&repr_sds, 123456789012L);
  if(strcmp("123456789012", repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}

int json_helpers_tests() {
  int failure_count = 0;
  failure_count += test_ul_arr_repr();
  failure_count += test_array_start();
  failure_count += test_array_next_item();
  failure_count += test_array_end();
  failure_count += test_dict_start();
  failure_count += test_dict_next_key();
  failure_count += test_dict_end();
  failure_count += test_dict_with_normal_key();
  failure_count += test_dict_with_quote_in_key();
  failure_count += test_dict_with_many_escape_chars();
  failure_count += test_string_repr();
  failure_count += test_time_repr();
  failure_count += test_unsigned_long_repr();

  return failure_count;
}
