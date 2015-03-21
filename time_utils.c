#include <string.h> //For NULL

#include <sys/time.h>

#include "time_utils.h"



/*****INTERFACE IMPLEMENTATION*/
unsigned long VARZMinutesSinceEpoch(varz_time_t time) {
  return time / SEC_IN_MIN; //May need to do something about leap seconds
}

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
