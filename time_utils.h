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

#endif
