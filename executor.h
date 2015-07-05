/* GOAL OF THE EXECUTOR
 * The basic processing pipeline for incoming commands is as follows
 * 1. The command is converted to a VARZOperationDescription
 * 2. The VARZOperationDescription is passed to the executor
 * 3. The executor (containing all persistent state) executes the command by calling the
 *    appropriate sub commands.
 * 4. The exeuctor passes any results back and these are optionally returned to the client
 */


#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include "command_description.h"
#include "hash_table.h"


#define VARZ_EXECUTOR_NUM_HASH_TABLES 2


struct VARZExecutorMetadata {
  // Note that unlike other times in varz, this is a local time (not one supplied by the client)
  varz_time_t executor_start_time;
};


typedef struct {
  struct VARZExecutorMetadata metadata;

  VARZHashTable_t mht_counters_ht, mht_samplers_ht;
} VARZExecutor_t;


void VARZExecutorInit(VARZExecutor_t *executor, unsigned long hash_table_size);
void VARZExecutorFree(VARZExecutor_t *executor);

/* Optionally passes back a return value as the void*, may do something less lame later
 */ 
void *VARZExecutorExecute(VARZExecutor_t *executor, struct VARZOperationDescription *op);


#endif
