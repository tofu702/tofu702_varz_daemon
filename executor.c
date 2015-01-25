#include <assert.h>
#include <stdlib.h>
#include <strings.h>


#include "executor.h"
#include "hash_funcs.h"
#include "hash_table.h"


#define DEFAULT_START_TIME 0
#define DEFAULT_SAMPLER_SIZE 1000


/***** STATIC HELPER PROTOTYPES *****/
void handleMHTCounterAdd(VARZExecutor_t *executor, struct VARZOperationDescription *op);
void handleMHTSamplerAdd(VARZExecutor_t *executor, struct VARZOperationDescription *op);


/***** INTERFACE IMPLEMENTATION *****/

void VARZExecutorInit(VARZExecutor_t *executor, unsigned long hash_table_size) {
  bzero(executor, sizeof(VARZExecutor_t));
  VARZHashTableInit(&(executor->mht_counters_ht), hash_table_size);
  VARZHashTableInit(&(executor->mht_samplers_ht), hash_table_size);
}

void VARZExecutorFree(VARZExecutor_t *executor) {
  // TODO: Go through the hash table and free its contents
  VARZHashTableFree(&(executor->mht_counters_ht));
  VARZHashTableFree(&(executor->mht_samplers_ht));
}


void *VARZExecutorExecute(VARZExecutor_t *executor, struct VARZOperationDescription *op) {
  // TODO Come up with a less stupid way of doing this
  switch (op->op) {
    case VARZOP_INVALID:
      return NULL;
    case VARZOP_MHT_COUNTER_ADD:
      handleMHTCounterAdd(executor, op);
      return NULL;
    case VARZOP_MHT_SAMPLE_ADD:
      handleMHTSamplerAdd(executor, op);
      return NULL;
  }

  // We got an invalid op...
  assert(0);
}


/***** STATIC HELPERS *****/
void handleMHTCounterAdd(VARZExecutor_t *executor, struct VARZOperationDescription *op) {
  VARZMHTIntCounter_t *counter;
  struct VARZMHTCounterAddOp *counter_add_op;
  uint64_t name_hash = VARZHashString(op->variable_name);
  counter = VARZHashTableGet(&(executor->mht_counters_ht), op->variable_name, name_hash);

  // Check if it exists
  if (!counter) {
    counter = calloc(1, sizeof(VARZMHTIntCounter_t));
    VARZMHTIntCounterInit(counter, DEFAULT_START_TIME);
    VARZHashTableAdd(&(executor->mht_counters_ht), op->variable_name, name_hash, counter);
  }

  counter_add_op = &(op->op_data.counter_add_op);
  VARZMHTIntCounterIncrement(counter, counter_add_op->time, counter_add_op->amt);
}


void handleMHTSamplerAdd(VARZExecutor_t *executor, struct VARZOperationDescription *op) {
  VARZMHTIntSampler_t *sampler;
  struct VARZMHTSamplerAddOp *sampler_add_op;
  uint64_t name_hash = VARZHashString(op->variable_name);
  sampler = VARZHashTableGet(&(executor->mht_samplers_ht), op->variable_name, name_hash);

  if (!sampler) {
    sampler = calloc(1, sizeof(VARZMHTIntSampler_t));
    VARZMHTIntSamplerInit(sampler, DEFAULT_START_TIME, DEFAULT_SAMPLER_SIZE);
    VARZHashTableAdd(&(executor->mht_samplers_ht), op->variable_name, name_hash, sampler);
  }
  sampler_add_op = &(op->op_data.sampler_add_op);
  VARZMHTIntSamplerAddSample(sampler, sampler_add_op->time, sampler_add_op->value,
                             sampler_add_op->random_vals);
}

