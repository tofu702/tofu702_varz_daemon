#ifndef TIME_UTILS
#define TIME_UTILS


#define SEC_IN_MIN 60
#define MIN_IN_HOUR 60
#define HOURS_IN_DAY 24

typedef unsigned long varz_time_t;

unsigned long VARZMinutesSinceEpoch(varz_time_t time);

varz_time_t VARZCurrentTime();

/**** DEBUGGING & TESTING ONLY *****/
varz_time_t VARZMakeTime(unsigned long days, unsigned long hours, unsigned long min, 
                         unsigned long sec);
double VARZCurrentDoubleTime();

#ifdef VARZ_STUB

/* IF VARZ_STUB is set (usually via gcc) this allows us to set the time returned by VARZCurrentTime
 * but it will ***NOT*** set the time returned by VARZCurrentDoubleTime (which is used only for
 * debugging testing and benchmarking.
 */
void VARZCurrentTimeStubValue(varz_time_t value);


#endif

#endif
