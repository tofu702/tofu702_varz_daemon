#ifndef COMMAND_DESCRIPTION_H_
#define COMMAND_DESCRIPTION_H_

#include "counter_variable.h"
#include "hash_table.h"
#include "sample_variable.h"


/***** COMMAND MIDDLEWARE *****/
/* Basic Idea Here: Create a middleware layer that any front end can use and dispatch with */
enum VARZOperationType {  
  VARZOP_INVALID = -1,
  VARZOP_MHT_COUNTER_ADD = 1,
  VARZOP_MHT_SAMPLE_ADD = 2
} ;

struct VARZMHTCounterAddOp {
  varz_time_t time;
  unsigned int amt;
};

struct VARZMHTSamplerAddOp {
  varz_time_t time; 
  unsigned long value;
  uint64_t random_vals[2];
};

union VARZOperationData {
  struct VARZMHTCounterAddOp counter_add_op;
  struct VARZMHTSamplerAddOp sampler_add_op;
};

struct VARZOperationDescription {
  enum VARZOperationType op;
  char variable_name[VARZ_HASHTABLE_MAX_NAME_LEN];
  union VARZOperationData op_data;
};


#endif

