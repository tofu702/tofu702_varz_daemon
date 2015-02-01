#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "all_tests.h"
#include "counter_variable.h"

static int test_counter_trivial_increments() {
  VARZMHTIntCounter_t counter; 
  unsigned long base_sec_since_epoch = VARZMakeTime(10, 5, 0, 0);
  VARZMHTIntCounterInit(&counter, base_sec_since_epoch);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch, 1);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 1, 10);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch+60, 4);

  if (counter.all_time_count != 15) {
    printf("Error test_counter_trivial_increments, all_time_count should be 15, got %lu\n",
           counter.all_time_count);
    return 1;
  } else if (counter.latest_time != base_sec_since_epoch + 60){
    return 1;
  } else if(counter.min_counters[0] != 11) {
    printf("Error test_counter_trivial_increments, min_counters[0] should be 11, got %lu\n",
           counter.min_counters[0]);
    return 1;
  } else if(counter.min_counters[1] != 4) {
    return 1;
  }
  return 0;
}


static int test_counter_with_older_values() {
  VARZMHTIntCounter_t counter; 
  unsigned long base_sec_since_epoch = VARZMakeTime(10, 5, 0, 30);
  VARZMHTIntCounterInit(&counter, base_sec_since_epoch);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch - 10, 1);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch - 31, 2);
  if(counter.all_time_count != 3) {
    printf("Error test_counter_with_older_values, all_time_count should be 3, got %lu\n",
           counter.all_time_count);
    return 1;
  } else if(counter.latest_time != base_sec_since_epoch) {
    printf("Error test_counter_with_older_values, latest_time should be %lu, got %lu\n",
           base_sec_since_epoch, counter.latest_time);
    return 1;
  } else if(counter.min_counters[0] != 1) {
    return 1;
  } else if(counter.min_counters[59] != 2) {
    return 1;
  }
  return 0;
}


static int test_count_pruning() {
  VARZMHTIntCounter_t counter; 
  unsigned long base_sec_since_epoch = VARZMakeTime(0, 60, 0, 0);
  VARZMHTIntCounterInit(&counter, base_sec_since_epoch);

  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch, 1);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 1800, 2);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 3600, 3);

  int sum = 0;
  for (int i=0; i < MIN_IN_HOUR; i++) 
    sum += counter.min_counters[i];

  if (counter.all_time_count != 6) {
    printf("ERROR test_count_pruning, all_time_count should be 6, got %lu\n",
           counter.all_time_count);
    return 1;
  } else if(counter.latest_time != base_sec_since_epoch + 3600) {
    printf("ERROR test_count_pruning, latest_time should be %lu, got %lu\n",
           base_sec_since_epoch + 3600, counter.all_time_count);
    return 1;
  } else if(counter.min_counters[0] != 3) {
    printf("ERROR test_count_pruning, min_counters[0] should be 3, got %lu\n",
           counter.min_counters[0]);
    return 1;
  } else if(counter.min_counters[30] != 2) {
    VARZMHTIntCounterPrint(&counter);
    printf("ERROR test_count_pruning, min_counters[30] should be 2, got %lu\n",
           counter.min_counters[30]);
    return 1;
  } else if(sum != 5) {
    printf("ERROR test_count_pruning, sum should be 5, got %d\n",
           sum);
    return 1;
  }
  return 0;
}

static int test_counter_pruning_isnt_off_by_one_on_low_side() {
  VARZMHTIntCounter_t counter; 
  unsigned long base_sec_since_epoch = VARZMakeTime(0, 60, 0, 0);
  VARZMHTIntCounterInit(&counter, base_sec_since_epoch);

  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch, 1);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 60*59+59, 2);

  int sum = 0;
  for (int i=0; i < MIN_IN_HOUR; i++) 
    sum += counter.min_counters[i];

  if (counter.min_counters[0] != 1) {
    printf("ERROR test_counter_pruning_isnt_off_by_one_on_low_side, min_counters[0] should be 1," 
           " got %lu\n", counter.min_counters[0]);
    return 1;
  } else if (counter.min_counters[59] != 2) {
    printf("ERROR test_counter_pruning_isnt_off_by_one_on_low_side, min_counters[59] should be 2," 
           " got %lu\n", counter.min_counters[59]);
    return 1; 
  } else if (sum != 3) {
    printf("ERROR test_counter_pruning_isnt_off_by_one_on_low_side, sum should be 3, got %d\n",
           sum);
    return 1;
  }

  return 0;
}

static int test_counter_pruning_isnt_off_by_one_on_high_side() {
  VARZMHTIntCounter_t counter; 
  unsigned long base_sec_since_epoch = VARZMakeTime(0, 60, 0, 0);
  VARZMHTIntCounterInit(&counter, base_sec_since_epoch);

  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch, 1);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 60*61+59, 2);

  int sum = 0;
  for (int i=0; i < MIN_IN_HOUR; i++) 
    sum += counter.min_counters[i];

  if (counter.min_counters[0] != 0) {
    printf("ERROR test_counter_pruning_isnt_off_by_one_on_high_side, min_counters[0] should be 0,"
           " got %lu\n", counter.min_counters[0]);
    return 1;
  } else if (counter.min_counters[1] != 2) {
    return 1; 
  } else if (sum != 2) {
    return 1;
  }

  return 0;
}

static int test_counter_pruning_with_non_zero_start_minute() {
  VARZMHTIntCounter_t counter; 
  unsigned long base_sec_since_epoch = VARZMakeTime(0, 60, 10, 0);
  VARZMHTIntCounterInit(&counter, base_sec_since_epoch);

  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch, 1);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 900, 2);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 3660, 3);

  int sum = 0;
  for (int i=0; i < MIN_IN_HOUR; i++) 
    sum += counter.min_counters[i];
  
  if (counter.min_counters[10] != 0) {
    return 1;
  } else if(counter.min_counters[25] != 2) {
    return 1;
  } else if(counter.min_counters[11] != 3) {
    return 1;
  } else if (sum != 5) {
    return 1;
  }

  return 0;
}


static int test_counter_json_repr() {
  sds repr_sds;
  VARZMHTIntCounter_t counter; 
  unsigned long base_sec_since_epoch = VARZMakeTime(0, 60, 0, 0);
  VARZMHTIntCounterInit(&counter, base_sec_since_epoch);

  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch, 1);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 1800, 2);
  VARZMHTIntCounterIncrement(&counter, base_sec_since_epoch + 3600, 3);

  repr_sds = sdsempty();

  VARZMHTIntCounterJSONRepr(&counter, &repr_sds);

  const char *desired_output = "{\"min_counters\":[3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"
     "0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"
     "\"all_time_count\":6,\"latest_time\":219600}";
  if (strcmp(desired_output, repr_sds)) {
    return 1;
  }
  sdsfree(repr_sds);
  return 0;
}


int counter_variable_tests() {
  int failure_count = 0;
  failure_count += test_counter_trivial_increments();
  failure_count += test_counter_with_older_values();
  failure_count += test_count_pruning();
  failure_count += test_counter_pruning_isnt_off_by_one_on_low_side();
  failure_count += test_counter_pruning_isnt_off_by_one_on_high_side();
  failure_count += test_counter_pruning_with_non_zero_start_minute();
  failure_count += test_counter_json_repr();

  return failure_count;
}
