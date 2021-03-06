#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_helpers.h"
#include "numeric_utils.h"
#include "sample_variable.h"

/****STATIC HELPER PROTOTYPES*****/
static bool shouldWeReplaceElement(unsigned long num_events_inc_this_one, unsigned long samples_size,
    uint32_t random_value);

/***** VARZMHTIntSample Implementation *****/
void VARZMHTIntSamplerInit(VARZMHTIntSampler_t *sampler, varz_time_t start_time,
                           unsigned int samples_per_set) {
  memset(sampler, 0, sizeof(VARZMHTIntSampler_t));
  VARZIntSampleSetInit(&(sampler->min_samples), samples_per_set);
  VARZIntSampleSetInit(&(sampler->all_time_samples), samples_per_set);
  sampler->latest_time = start_time;
}

void VARZMHTIntSamplerFree(VARZMHTIntSampler_t *sampler) {
  VARZIntSampleSetFree(&(sampler->min_samples));
  VARZIntSampleSetFree(&(sampler->all_time_samples));
}


void VARZMHTIntSamplerAddSample(VARZMHTIntSampler_t *sample, varz_time_t sample_time,
  unsigned long sample_value, uint64_t random_vals[2]) {
  VARZIntSampleAddSample(&(sample->all_time_samples), sample_time, sample_value, random_vals[0]);

  unsigned long latest_min, sample_min;
  latest_min = VARZMinutesSinceEpoch(sample->latest_time);
  sample_min = VARZMinutesSinceEpoch(sample_time);
  
  if (sample_min > latest_min) {
    // We clear the minute if new sample arrived in a newer minute
    VARZIntSampleSetClear(&(sample->min_samples));
    VARZIntSampleAddSample(&(sample->min_samples), sample_time, sample_value, random_vals[1]);
  } else if (sample_min == latest_min) {
    // If they are the same minute, just add it
    VARZIntSampleAddSample(&(sample->min_samples), sample_time, sample_value, random_vals[1]);
  } //Otherwise: We'll drop this sample since it doesn't correspond to the most recent minute

  sample->latest_time = MAX(sample->latest_time, sample_time);
}


void VARZMHTIntSamplerJSONRepr(VARZMHTIntSampler_t *sampler, sds *dest) {
  VARZJSONDictStart(dest);

  VARZJSONDictKey(dest, "latest_time_sec");
  VARZJSONTimeRepr(dest, sampler->latest_time);

  VARZJSONDictNextKey(dest);
  VARZJSONDictKey(dest, "last_minute_samples");
  VARZIntSampleSetJSONRepr(&(sampler->min_samples), dest);

  VARZJSONDictNextKey(dest);
  VARZJSONDictKey(dest, "all_time_samples");
  VARZIntSampleSetJSONRepr(&(sampler->all_time_samples), dest);

  VARZJSONDictEnd(dest);
}


/***** VARZIntSampleSet Implementation *****/
void VARZIntSampleSetInit(VARZIntSampleSet_t *sample_set, unsigned long samples_size) {
  memset(sample_set, 0, sizeof(VARZIntSampleSet_t));
  sample_set->samples_size = samples_size;
  sample_set->samples = calloc(samples_size, sizeof(VARZIntSample_t));
}


void VARZIntSampleSetClear(VARZIntSampleSet_t *sample_set) {
  sample_set->num_events = 0;
  //NOTE: We could also bzero samples, but for the sake of performance, we won't
}


void VARZIntSampleSetFree(VARZIntSampleSet_t *sample_set) {
  free(sample_set->samples);
  // Aren't strictly needed, but we'll do them to prevent any future errors
  sample_set->samples = NULL;
  sample_set->samples_size = 0;
}


void VARZIntSampleAddSample(VARZIntSampleSet_t *sample_set, varz_time_t sample_time,
    unsigned long sample_value, uint64_t random_value) {
  assert(sample_set->samples);

  VARZIntSample_t *dest = NULL;
  if (sample_set->num_events < sample_set->samples_size) {
    // If there are less than num_sample values, insert linearly
    dest = sample_set->samples + sample_set->num_events;
  } else {
    // Otherwise determine if we need to replace a sample
    uint32_t upper_half = random_value >> 32;
//    printf("TODO REMOVE: upper: %x\n", upper_half);
    if (shouldWeReplaceElement(sample_set->num_events+1, sample_set->samples_size, upper_half)) {
      uint32_t lower_half = (random_value << 32) >> 32;
      unsigned long replacement_index = (unsigned long) lower_half % sample_set->samples_size;
//      printf("DEBUG: replacement_index=%lu, sample_time=%lu, sample_value=%lu\n", replacement_index,
//            sample_time, sample_value);
      dest = sample_set->samples + replacement_index;
    }
  }
  if (dest) {
    dest->sample_value = sample_value;
    dest->sample_time = sample_time;
  }
  sample_set->num_events ++;
}


void VARZIntSampleSetJSONRepr(VARZIntSampleSet_t *sample_set, sds *dest) {
  sds values_sds, time_sds;
  values_sds = sdsempty();
  time_sds = sdsempty();
  values_sds = sdsMakeRoomFor(values_sds, sample_set->num_events * 16);
  time_sds = sdsMakeRoomFor(time_sds, sample_set->num_events * 16);

  VARZJSONArrayStart(&values_sds);
  VARZJSONArrayStart(&time_sds);
  // Only bother serializing the samples we have
  for (int i=0; i < MIN(sample_set->samples_size, sample_set->num_events); i++) {
    if (i != 0) {
      VARZJSONArrayNextItem(&values_sds);
      VARZJSONArrayNextItem(&time_sds);
    }
    VARZIntSample_t *item = &(sample_set->samples[i]);
    VARZJSONUnsignedLongRepr(&values_sds, item->sample_value);
    VARZJSONUnsignedLongRepr(&time_sds, item->sample_time);
  }
  VARZJSONArrayEnd(&values_sds);
  VARZJSONArrayEnd(&time_sds);

  VARZJSONDictStart(dest);
  VARZJSONDictKey(dest, "sample_values");
  *dest = sdscatsds(*dest, values_sds);

  VARZJSONDictNextKey(dest);
  VARZJSONDictKey(dest, "sample_times_sec");
  *dest = sdscatsds(*dest, time_sds);

  VARZJSONDictNextKey(dest);
  VARZJSONDictKey(dest, "samples_size");
  VARZJSONUnsignedLongRepr(dest, sample_set->samples_size);

  VARZJSONDictNextKey(dest);
  VARZJSONDictKey(dest, "num_events");
  VARZJSONUnsignedLongRepr(dest, sample_set->num_events);

  VARZJSONDictEnd(dest);
  sdsfree(values_sds);
  sdsfree(time_sds);
}

/***** VARZIntSampleSet Static Helpers *****/
static bool shouldWeReplaceElement(unsigned long num_events_inc_this_one, unsigned long samples_size,
    uint32_t random_value) {
  assert(num_events_inc_this_one >= samples_size);
  // Odds of this sample being allowed in: sample_size / num_events
  // That is: replacement should occur samples_size / num_events fraction of the time
  // Alternative statement: Odds of the sample not being allowed in are 1 - (sample_size / num_events)
  // That is: replacement should not occur (num_events - sample_size) / num_events fraction of the time
  // So: If the (random_value / UINT32_MAX) < (num_events - sample_size) / num_events, we should not replace
  // Or: If (random_value / UINT32_MAX) > (num_events - sample_size) / num_events,  we should replace
  // IE: Replace if random_value > UINT32_MAX * (num_events-sample_size) / num_events

  unsigned long acceptance_bound = (((num_events_inc_this_one - samples_size) * UINT32_MAX) \
                                 / num_events_inc_this_one);

//  printf("DEBUG: %lu / %lu = %f\n", samples_size, num_events_inc_this_one, ((float)samples_size / num_events_inc_this_one));
//  printf("DEBUG: \t Bound=%lu\n",  (((num_events_inc_this_one - samples_size) * UINT32_MAX) / num_events_inc_this_one));
//  printf("DEBUG: rv=%u (%f%%)\n", random_value, (float)random_value / UINT32_MAX);

  return random_value > acceptance_bound;
}

