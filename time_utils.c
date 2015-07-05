#include <string.h> //For NULL

#include <sys/time.h>

#include "time_utils.h"



/*****INTERFACE IMPLEMENTATION*/
unsigned long VARZMinutesSinceEpoch(varz_time_t time) {
  return time / SEC_IN_MIN; //May need to do something about leap seconds
}


#ifndef VARZ_STUB

varz_time_t VARZCurrentTime() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec;
}

#endif

varz_time_t VARZMakeTime(unsigned long days, unsigned long hours, unsigned long min, 
                         unsigned long sec) {
  hours += days * HOURS_IN_DAY;
  min += hours * MIN_IN_HOUR;
  sec += min * SEC_IN_MIN;

  return sec;
}

double VARZCurrentDoubleTime() {
  struct timeval tp;
  gettimeofday(&tp, NULL);

  return ((double)tp.tv_sec) + ((double)tp.tv_usec/1.0e6);
}

/*****STUB IMPLEMENTATION*****/

#ifdef VARZ_STUB

varz_time_t varz_current_time_stub_value = 0;

varz_time_t VARZCurrentTime() {
  return varz_current_time_stub_value;
}

void VARZCurrentTimeStubValue(varz_time_t value) {
  varz_current_time_stub_value = value;
}

#endif
