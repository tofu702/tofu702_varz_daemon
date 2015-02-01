#include <assert.h>
#include <stdio.h>
#include <strings.h>

#include "sds/sds.h"

#include "counter_variable.h"
#include "json_helpers.h"
#include "numeric_utils.h"
#include "time_utils.h"

/*****NOTES*****/
/* Invariants & rules for counters: 
 * 1. min_counters represent the 60 minutes that occurred before latest_time
 * 2. latest_time is always the largest time value we've seen
 * 2. If an update comes from a time older than 1 hour ago, it is discarded
 * 3. If an update is the largest, time is advanced and any buckets between the prior "latest_time"
      and the current sample's time are zeroed. Latest_time is advanced
 * 4. If an update lands between the "latest_time" and 1 hour prior, the corresponding bucket is
      is computed and updated
 */ 

/*****STATIC HELPER PROTOTYPES*****/
static unsigned long convertIdxToMinutes(VARZMHTIntCounter_t *counter, unsigned long idx);
static void pruneToCurrentTime(VARZMHTIntCounter_t *counter, varz_time_t newest_time);


/*****INTERFACE IMPLEMENTATION*****/

void VARZMHTIntCounterInit(VARZMHTIntCounter_t *counter, varz_time_t start_time) {
  bzero(counter, sizeof(VARZMHTIntCounter_t));
  counter->latest_time = start_time;
}

void VARZMHTIntCounterIncrement(VARZMHTIntCounter_t *counter, varz_time_t sample_time, unsigned int amt) {
  unsigned long sample_min_since_epoch = VARZMinutesSinceEpoch(sample_time);
  unsigned long counter_min_since_epoch = VARZMinutesSinceEpoch(counter->latest_time);
 
  // We always update the all time count
  counter->all_time_count += amt;

  if(sample_min_since_epoch < counter_min_since_epoch - MIN_IN_HOUR) {
    //This sample is more than 1 hour old, discard it and return
    return;
  }
  if (sample_time > counter->latest_time) {
    // This is the largest seen, let's advance time to it
    pruneToCurrentTime(counter, sample_time);
    counter->latest_time = sample_time;
  }
  unsigned long min_idx = sample_min_since_epoch % MIN_IN_HOUR;
  counter->min_counters[min_idx] += amt;
}


void VARZMHTIntCounterJSONRepr(VARZMHTIntCounter_t *counter, sds *dest) {
 /* int min_counters_repr_size = VARZJSONBytesNeededForList(MIN_IN_HOUR, JSON_BYTES_NEEDED_FOR_UL);
  char min_counters_repr[min_counters_repr_size];
  int all_time_count_repr_size = JSON_BYTES_NEEDED_FOR_UL;
  int latest_time_repr_size = JSON_BYTES_NEEDED_FOR_TIME;
  char latest_time_repr[latest_time_repr_size];
  int fudge_size = 64;
  int total_required_size = min_counters_repr_size + all_time_count_repr_size + latest_time_repr_size + fudge_size;
  char *overall_repr = calloc(total_required_size, sizeof(char));
  VARZJSONUnsignedLongArrToRepr(counter->min_counters, MIN_IN_HOUR, min_counters_repr,
                                min_counters_repr_size);
  VARZJSONTimeRepr(counter->latest_time, latest_time_repr, latest_time_repr_size);
  sprintf("{min_counters: %s, all_time_count: %llu, latest_time: %s}", min_counters_repr, 
          counter->all_time_count, latest_time_repr);*/
  VARZJSONDictStart(dest);
  VARZJSONDictKey(dest, "min_counters");
  VARZJSONUnsignedLongArrToRepr(dest, counter->min_counters, MIN_IN_HOUR);

  VARZJSONDictNextKey(dest);
  VARZJSONDictKey(dest, "all_time_count");
  VARZJSONUnsignedLongRepr(dest, counter->all_time_count);

  VARZJSONDictNextKey(dest);
  VARZJSONDictKey(dest, "latest_time");
  VARZJSONTimeRepr(dest, counter->latest_time);

  VARZJSONDictEnd(dest);
}


void VARZMHTIntCounterPrint(VARZMHTIntCounter_t *counter) {
  printf("all_time_count: %lu\n", counter->all_time_count);
  printf("latest_time: %lu s (%lu min)\n", counter->latest_time, 
      VARZMinutesSinceEpoch(counter->latest_time));
  for (int i=0; i < MIN_IN_HOUR; i++) {
    unsigned long min_since_epoch = convertIdxToMinutes(counter, i);
    printf("\t min %02d (%lu min): %lu\n", i, min_since_epoch, counter->min_counters[i]);
  }
}


/*****STATIC HELPERS*****/
static unsigned long convertIdxToMinutes(VARZMHTIntCounter_t *counter, unsigned long idx) {
  unsigned long latest_min = VARZMinutesSinceEpoch(counter->latest_time);
  unsigned long latest_min_idx = latest_min % MIN_IN_HOUR;

  // The idx after latest_min_idx will be the oldest, the one before will be the second newest
  // Mathematical Truths: 
  // 1. Only minutes from latest_min - 59 to latest_min inclusive exist
  // 2. The position of a given minute is given by idx = (min % MIN_IN_HOUR)
  // Thus, the question is: what number betweem lastest_min - 59 and latest_min = idx
  // We know that i_l = latest_min_idx
  // Let i_x = index of a minute x min *BEFORE* latest_min
  // We know that i_x = i_l - x for all x <= i_l
  // We also that i_x = (i_l + 60 - x) for all x >= -l
  // That is: i_x = (i_l - x) % 60
  // OR: index_x = (index_latest - (latest_min - minute_x)) % MIN_IN_HOUR/
  //
  // Now let's talk about how to go in the reverse
  // for x <= i_l: x = i_l - i_x
  // for x > i_l : x = i_l + 60 - i_x = i_l - i_x + 60
  // That is: x = (i_l - i_x) % 60
  // OR: (latest_min - minute_x) = (index_latest - index_x) % 60
  // Therefore minute_x = latest_min - (index_latest - index_x) % 60

//  printf("latest_min=%lu, latest_min_idx=%lu, idx=%lu\n", latest_min, latest_min_idx, idx);
//  printf("computed_minute: %lu\n", latest_min - ((60 + latest_min_idx - idx) % 60));
  return latest_min - ((60 + latest_min_idx - idx) % 60);
}


static void pruneToCurrentTime(VARZMHTIntCounter_t *counter, varz_time_t newest_time) {
  // Structure currently contains everything from counter->latest_time - 60 min to counter->latest_time
  // We now have a new newest_time
  // Num buckets to toss = min(60, minutes_passed)
  // We should start at oldest_time and zero mins from oldest_time to num_buckets to toss

  assert(newest_time > counter->latest_time);

  unsigned long latest_min_in_counter = VARZMinutesSinceEpoch(counter->latest_time);
  
  //+1 since inclusive
  unsigned long oldest_min_in_counter = latest_min_in_counter - MIN_IN_HOUR + 1;

  unsigned long oldest_min_in_counter_idx = oldest_min_in_counter % MIN_IN_HOUR;
  unsigned long newest_time_min = VARZMinutesSinceEpoch(newest_time);
  unsigned long num_minutes_to_zero = MIN(newest_time_min - latest_min_in_counter, MIN_IN_HOUR);

  /*
  printf("oldest_min_in_counter=%lu, oldest_min_in_counter_idx=%lu\n", oldest_min_in_counter, 
           oldest_min_in_counter_idx);
  printf("latest_min_in_counter=%lu, latest_min_in_counter_idx=%lu\n", latest_min_in_counter, 
      latest_min_in_counter % MIN_IN_HOUR);
  printf("newest_time_min=%lu, num_minutes_to_zero=%lu\n\n", newest_time_min, num_minutes_to_zero);
  */

  for (int i=0; i < num_minutes_to_zero; i++) {
    //TODO: BZero might be faster
    int min_idx = (oldest_min_in_counter_idx + i) % MIN_IN_HOUR;
    counter->min_counters[min_idx] = 0;
  }
}
