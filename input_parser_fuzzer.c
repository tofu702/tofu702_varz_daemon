#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "all_fuzzers.h"
#include "command_description.h"
#include "input_parser.h"

#define EXECUTOR_HT_SIZE 1000

// Including unprintables, but is always null terminated
static void random_string_of_len(char *dest, int len) {
  for(int i=0; i < len-2; i++) {
    dest[i] = rand() % 256;
  }
  dest[len-2] = ';';
  dest[len-1] = '\0';
}

static void random_printable_string_of_len(char *dest, int len) {
  char smallest=' ', largest='~';
  int num_chars = largest-smallest;
  for(int i=0; i < len-1; i++) {
    dest[i] = (rand() % num_chars) + smallest;
  }
  dest[len-1] = '\0';
  
}

// Will always produce a length at least 2. We need the last spot for the \0
// The second to last spot it to shove in a ; if desired by the caller
// Yes, it's a hack, but what the heck, it's a fuzzer
static void random_printable_string_of_rand_len(char *dest, int maxlen) {
  assert(maxlen >= 2);
  int len = (rand() % (maxlen-2)) + 2;
  random_printable_string_of_len(dest, len);
}

static int fuzz_random_input() {
  char random_str[4096];
  int len_this_time;
  struct VARZOperationDescription desc;
  
  // Len must be at least 2
  len_this_time = rand() % (sizeof(random_str)-2) + 2;

  random_string_of_len(random_str, len_this_time);
  desc = VARZOpCmdParse(random_str, len_this_time);
  if (desc.op != VARZOP_INVALID) {
    return 1;
  }
  return 0;
}

static int fuzz_printable_random_input() {
  char random_str[4096];
  int len;
  struct VARZOperationDescription desc;

  random_printable_string_of_rand_len(random_str, sizeof(random_str));
  len = strlen(random_str);
  random_str[len-2] = ';';

  desc = VARZOpCmdParse(random_str, strlen(random_str));
  if (desc.op != VARZOP_INVALID) {
    return 1;
  }
  return 0;
}

static void change_spaces_to_understore(char *s) {
  for (int i=0; s[i]; i++) {
    if (s[i] == ' ') {
      s[i] = '_';
    }
  }
}

// op_name is currently either VARZ_MHT_COUNTER_ADD_OP_NAME, VARZ_MHT_SAMPLE_ADD_OP_NAME
static int fuzz_all_params_to_name_time_value_function(char *op_name, enum VARZOperationType t) {
  struct VARZOperationDescription desc;
  char command_str[4096], fake_name[512], fake_time[128], fake_value[128];
  random_printable_string_of_rand_len(fake_name, sizeof(fake_name));
  random_printable_string_of_rand_len(fake_time, sizeof(fake_time));
  random_printable_string_of_rand_len(fake_value, sizeof(fake_value));

  sprintf(command_str, "%s %s %s %s;", op_name, fake_name, fake_time, fake_value);

  desc = VARZOpCmdParse(command_str, strlen(command_str));

  // Yes, this check isn't very rigorous, desc.op is seldom t but it sometimes happens...
  if (desc.op != VARZOP_INVALID && desc.op != t) {
    return 1;
  }
  return 0;
}

static int fuzz_counter_add_with_weird_name_only() {
  struct VARZOperationDescription desc;
  char command_str[4096], fake_name[256];
  int fake_time, fake_value; 
  random_printable_string_of_rand_len(fake_name, sizeof(fake_name));
  change_spaces_to_understore(fake_name);
  fake_time = rand();
  fake_value = rand();

  sprintf(command_str, "%s %s %d %d;", VARZ_MHT_COUNTER_ADD_OP_NAME, fake_name, fake_time, fake_value);

  desc = VARZOpCmdParse(command_str, strlen(command_str));
  if (desc.op == VARZOP_MHT_COUNTER_ADD) {
    // Check that it actually parsed correctly...
    if (strcmp(fake_name, desc.variable_name)) {
      printf("Names don't match '%s' != '%s'\n", fake_name, desc.variable_name);
      return 1;
    } else if(desc.op_data.counter_add_op.time != fake_time) {
      return 1;
    } else if(desc.op_data.counter_add_op.amt != fake_value) {
      return 1;
    }
    return 0;
  }
  if (desc.op != VARZOP_INVALID) {
    return 1;
  }
  return 0;
}

static int fuzz_sampler_add_with_weird_name_only() {
  struct VARZOperationDescription desc;
  char command_str[4096], fake_name[256];
  int fake_time, fake_value; 
  random_printable_string_of_rand_len(fake_name, sizeof(fake_name));
  change_spaces_to_understore(fake_name);
  fake_time = rand();
  fake_value = rand();

  sprintf(command_str, "%s %s %d %d;", VARZ_MHT_SAMPLE_ADD_OP_NAME, fake_name, fake_time, fake_value);

  desc = VARZOpCmdParse(command_str, strlen(command_str));
  if (desc.op == VARZOP_MHT_SAMPLE_ADD) {
    // Check that it actually parsed correctly...
    if (strcmp(fake_name, desc.variable_name)) {
      printf("Names don't match '%s' != '%s'\n", fake_name, desc.variable_name);
      return 1;
    } else if(desc.op_data.sampler_add_op.time != fake_time) {
      return 1;
    } else if(desc.op_data.sampler_add_op.value != fake_value) {
      return 1;
    }
    return 0;
  }
  if (desc.op != VARZOP_INVALID) {
    return 1;
  }
  return 0;
}

int input_parser_fuzzer(int num_iterations_per_step) {
  int failure_count = 0;

  for(int i=0; i < num_iterations_per_step; i++) {
    failure_count += fuzz_random_input();
    failure_count += fuzz_printable_random_input();
    failure_count += fuzz_all_params_to_name_time_value_function(VARZ_MHT_COUNTER_ADD_OP_NAME,
                                                                 VARZOP_MHT_COUNTER_ADD);
    failure_count += fuzz_counter_add_with_weird_name_only();
    failure_count += fuzz_all_params_to_name_time_value_function(VARZ_MHT_SAMPLE_ADD_OP_NAME,
                                                                 VARZOP_MHT_SAMPLE_ADD);
    failure_count += fuzz_sampler_add_with_weird_name_only();
  }

  return failure_count;
}

