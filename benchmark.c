#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "executor.h"
#include "random.h"
#include "time_utils.h"

#define BENCHMARK_HT_SIZE 1024
#define BENCHMARK_NUM_VAR_NAMES 2048
#define BENCHMARK_START_TIME VARZMakeTime(12, 1, 5, 0)
#define BENCHMARK_END_TIME VARZMakeTime(12, 2, 5, 0)
#define BENCHMARK_COUNTER_MAX_AMT 10

/***** STATIC HELPER PROTOTYPES *****/
static void generateRandomTime(varz_time_t *dest, varz_time_t start_time, varz_time_t end_time);
static struct VARZOperationDescription randomMHTCounterAddOp();
static struct VARZOperationDescription randomMHTSamplerAddOp();
static struct VARZOperationDescription allDumpJsonOp();
static void benchmark_executor();


int main(int argc, char **argv) {
  benchmark_executor();
  return 0;
}


/***** STATIC HELPERS *****/
static void generateRandomVariableName(char *dest, int num_names) {
  sprintf(dest, "variable_%d", rand() % num_names);
}

static void generateRandomTime(varz_time_t *dest, varz_time_t start_time, varz_time_t end_time) {
  varz_time_t time_delta;
  time_delta = end_time - start_time;
  *dest = rand() % time_delta + start_time;
}

static struct VARZOperationDescription randomMHTCounterAddOp() {
  struct VARZOperationDescription desc;

  desc.op = VARZOP_MHT_COUNTER_ADD;
  generateRandomVariableName(desc.variable_name, BENCHMARK_NUM_VAR_NAMES);
  generateRandomTime(&(desc.op_data.counter_add_op.time), BENCHMARK_START_TIME, BENCHMARK_END_TIME);
  desc.op_data.counter_add_op.amt = rand() % BENCHMARK_COUNTER_MAX_AMT;

  return desc;
}

static struct VARZOperationDescription randomMHTSamplerAddOp() {
  struct VARZOperationDescription desc;

  desc.op = VARZOP_MHT_SAMPLE_ADD;
  generateRandomVariableName(desc.variable_name, BENCHMARK_NUM_VAR_NAMES);
  generateRandomTime(&(desc.op_data.sampler_add_op.time), BENCHMARK_START_TIME, BENCHMARK_END_TIME);
  desc.op_data.sampler_add_op.value = rand();  // Doesn't matter at all
  desc.op_data.sampler_add_op.random_vals[0] = VARZRand64();
  desc.op_data.sampler_add_op.random_vals[1] = VARZRand64();

  return desc;
}

static struct VARZOperationDescription allDumpJsonOp() {
  struct VARZOperationDescription desc;
  desc.op = VARZOP_ALL_DUMP_JSON;
  return desc;
}


static struct VARZOperationDescription allListJsonOp() {
  struct VARZOperationDescription desc;
  desc.op = VARZOP_ALL_LIST_JSON;
  return desc;
}


static void benchmark_executor() {
  // Benchmark Only Adds
  VARZExecutor_t executor;
  struct VARZOperationDescription op;
  double start_time, end_time;
  char *json_dump, *json_list;

  const int num_commmands = 16*1024*1024;

  VARZExecutorInit(&executor, BENCHMARK_HT_SIZE);

  start_time = VARZCurrentDoubleTime();
  for (int i=0; i < num_commmands; i++) {
    switch (rand() % 2) {
      case 0:
        op = randomMHTCounterAddOp();
        break;
      default:
        op = randomMHTSamplerAddOp();
        break;
    }
    VARZExecutorExecute(&executor, &op);
  }
  end_time = VARZCurrentDoubleTime();
  
  printf("Executor Benchmark started at: %f, ended at: %f, elapsed: %f, ops per sec: %f\n", start_time, end_time,
         end_time-start_time, num_commmands/(end_time-start_time));

  // JSON DUMP
  start_time = VARZCurrentDoubleTime();
  op = allDumpJsonOp();
  json_dump = (char *) VARZExecutorExecute(&executor, &op);
  end_time = VARZCurrentDoubleTime();
  printf("JSON Dump Benchmark started at: %f, ended at: %f, elapsed: %f, len=%lu, MB/s:%f\n", start_time, 
         end_time, end_time-start_time, strlen(json_dump), strlen(json_dump) / 1024.0 / 1024.0 / (end_time-start_time));
  free(json_dump);

  // JSON LIST
  start_time = VARZCurrentDoubleTime();
  op = allListJsonOp();
  json_list = (char *) VARZExecutorExecute(&executor, &op);
  end_time = VARZCurrentDoubleTime();
  printf("JSON List Benchmark started at: %f, ended at: %f, elapsed: %f, len=%lu, MB/s:%f\n", start_time, 
         end_time, end_time-start_time, strlen(json_list), strlen(json_list) / 1024.0 / 1024.0 / (end_time-start_time));
  free(json_list);

  VARZExecutorFree(&executor);
}
