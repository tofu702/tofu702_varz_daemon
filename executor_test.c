#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "all_tests.h"
#include "command_description.h"
#include "executor.h"


static int test_executor_ignores_invalid_op() {
  VARZExecutor_t executor;
  struct VARZOperationDescription desc;
  VARZExecutorInit(&executor, 2);

  desc.op = VARZOP_INVALID;

  VARZExecutorExecute(&executor, &desc);

  if (executor.mht_counters_ht.total_entries != 0) {
    return 1;
  } else if(executor.mht_samplers_ht.total_entries != 0) {
    return 1;
  }
  VARZExecutorFree(&executor);
  return 0;
}


static int test_executor_with_new_mnt_counter_add_op() {
  VARZExecutor_t executor;
  struct VARZOperationDescription desc;
  VARZExecutorInit(&executor, 2);

  desc.op = VARZOP_MHT_COUNTER_ADD;
  strcpy(desc.variable_name, "foobar");
  desc.op_data.counter_add_op.time = 27;
  desc.op_data.counter_add_op.amt = 1;

  VARZExecutorExecute(&executor, &desc);

  if (executor.mht_counters_ht.total_entries != 1) {
    return 1;
  } else if(executor.mht_samplers_ht.total_entries != 0) {
    return 1;
  }

  VARZExecutorFree(&executor);
  return 0;
}


static int test_executor_with_repeat_mnt_counter_add_op() {
  VARZExecutor_t executor;
  struct VARZOperationDescription desc;
  VARZExecutorInit(&executor, 2);

  desc.op = VARZOP_MHT_COUNTER_ADD;
  strcpy(desc.variable_name, "foobar");
  desc.op_data.counter_add_op.time = 27;
  desc.op_data.counter_add_op.amt = 1;

  // First Execution
  VARZExecutorExecute(&executor, &desc);

  desc.op_data.counter_add_op.amt = 6; 
  desc.op_data.counter_add_op.time = 42;
  VARZExecutorExecute(&executor, &desc);

  if (executor.mht_counters_ht.total_entries != 1) {
    return 1;
  } else if(executor.mht_samplers_ht.total_entries != 0) {
    return 1;
  }

  VARZExecutorFree(&executor);
  return 0;
}


static int test_executor_with_new_mnt_sampler_add_op() {
  VARZExecutor_t executor;
  struct VARZOperationDescription desc;
  VARZExecutorInit(&executor, 2);

  desc.op = VARZOP_MHT_SAMPLE_ADD;
  strcpy(desc.variable_name, "foobaz");
  desc.op_data.sampler_add_op.time = 27;
  desc.op_data.sampler_add_op.value = 1;
  desc.op_data.sampler_add_op.random_vals[0] = 1;
  desc.op_data.sampler_add_op.random_vals[1] = 2;

  VARZExecutorExecute(&executor, &desc);

  if (executor.mht_counters_ht.total_entries != 0) {
    return 1;
  } else if(executor.mht_samplers_ht.total_entries != 1) {
    return 1;
  }

  VARZExecutorFree(&executor);
  return 0;
}


static int test_executor_with_repeat_mnt_sampler_add_op() {
  VARZExecutor_t executor;
  struct VARZOperationDescription desc;
  VARZExecutorInit(&executor, 2);

  desc.op = VARZOP_MHT_SAMPLE_ADD;
  strcpy(desc.variable_name, "foobaz");
  desc.op_data.sampler_add_op.time = 27;
  desc.op_data.sampler_add_op.value = 1;
  desc.op_data.sampler_add_op.random_vals[0] = 1;
  desc.op_data.sampler_add_op.random_vals[1] = 2;

  VARZExecutorExecute(&executor, &desc);
  
  desc.op_data.sampler_add_op.time = 42;
  desc.op_data.sampler_add_op.value = 11;
  VARZExecutorExecute(&executor, &desc);

  if (executor.mht_counters_ht.total_entries != 0) {
    return 1;
  } else if(executor.mht_samplers_ht.total_entries != 1) {
    return 1;
  }

  VARZExecutorFree(&executor);
  return 0;
}


int executor_tests() {
  int failure_count = 0;
  failure_count += test_executor_ignores_invalid_op();
  failure_count += test_executor_with_new_mnt_counter_add_op();
  failure_count += test_executor_with_repeat_mnt_counter_add_op();
  failure_count += test_executor_with_new_mnt_sampler_add_op();
  failure_count += test_executor_with_repeat_mnt_sampler_add_op();

  return failure_count;
}
