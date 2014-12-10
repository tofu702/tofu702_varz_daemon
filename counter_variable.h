#ifndef COUNTER_VARIABLE_H_
#define COUNTER_VARIABLE_H_

#include "time_utils.h"


typedef struct {
  unsigned long min_counters[MIN_IN_HOUR];
  unsigned long all_time_count;
  varz_time_t latest_time;
} VARZMHTIntCounter_t;


void VARZMHTIntCounterInit(VARZMHTIntCounter_t *counter, varz_time_t start_time);
void VARZMHTIntCounterIncrement(VARZMHTIntCounter_t *counter, varz_time_t sample_time, unsigned int amt);

/***** DEBUGGING & TESTING ONLY*****/
void VARZMHTIntCounterPrint(VARZMHTIntCounter_t *counter);

#endif
