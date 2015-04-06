#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "all_tests.h"
#include "sample_variable.h"

/***** HELPERS *****/
static VARZIntSample_t makeSample(varz_time_t sample_time, unsigned long sample_value) {
  VARZIntSample_t returnme;
  returnme.sample_time = sample_time;
  returnme.sample_value = sample_value;
  return returnme;
}

static bool samplesEqual(VARZIntSample_t a, VARZIntSample_t b) {
  return (a.sample_time == b.sample_time) && (a.sample_value == b.sample_value);
}

static uint64_t makeUINT64(uint32_t upper, uint32_t lower) {
  return ((uint64_t) upper) << 32 | ((uint64_t) lower);
}

// We return dest for convenience
static uint64_t* make2UINT64s(uint32_t upper1, uint32_t lower1, uint32_t upper2, uint32_t lower2, 
                              uint64_t dest[2]) {
  dest[0] = makeUINT64(upper1, lower1);
  dest[1] = makeUINT64(upper2, lower2);
  return dest;
}

/***** TESTS FOR INT SAMPLE VARIABLES *****/

static int test_int_sample_adds_until_full_without_replacement() {
  VARZIntSampleSet_t sample_set;
  VARZIntSampleSetInit(&sample_set, 3);
  VARZIntSampleAddSample(&sample_set, 101, 1, 90);
  VARZIntSampleAddSample(&sample_set, 102, 2, 90);
  VARZIntSampleAddSample(&sample_set, 103, 3, 90);
  

  if (sample_set.samples_size != 3) {
    return 1;
  } else if (sample_set.num_events != 3) {
    return 1;
  } else if (!samplesEqual(sample_set.samples[0], makeSample(101, 1))) {
    return 1;
  } else if(!samplesEqual(sample_set.samples[1], makeSample(102, 2))) {
    return 1;
  } else if(!samplesEqual(sample_set.samples[2], makeSample(103, 3))) {
    return 1;
  }
  VARZIntSampleSetFree(&sample_set);
  return 0;
}

static int test_does_replacement_if_score_is_high_enough() {
  VARZIntSampleSet_t sample_set;
  VARZIntSampleSetInit(&sample_set, 3);
  VARZIntSampleAddSample(&sample_set, 101, 1, 90);
  VARZIntSampleAddSample(&sample_set, 102, 2, 90);
  VARZIntSampleAddSample(&sample_set, 103, 3, 90);

  // At this point, we should have a 75% (3/4) chance of replacing
  // So a value that's 1/4th + 1 of MAX_UINT32 should result in replacement
  // We'll pick a value to replace with the victim being slot 2
  VARZIntSampleAddSample(&sample_set, 104, 4, makeUINT64((uint64_t) UINT32_MAX / 4+1, 2+12));
  
  if (sample_set.samples_size != 3) {
    return 1;
  } else if (sample_set.num_events != 4) {
    printf("Error test_does_replacement_if_score_is_high_enough, num_events should be 4, got %lu\n",
           sample_set.num_events);
    return 1;
  } else if (!samplesEqual(sample_set.samples[0], makeSample(101, 1))) {
    return 1;
  } else if(!samplesEqual(sample_set.samples[1], makeSample(102, 2))) {
    return 1;
  } else if(!samplesEqual(sample_set.samples[2], makeSample(104, 4))) {
    printf("Error test_does_replacement_if_score_is_high_enough, value at [2] should be (104, 4), " 
           " got (%lu,%lu)\n", sample_set.samples[2].sample_time, sample_set.samples[2].sample_value);
    return 1;
  }
  VARZIntSampleSetFree(&sample_set);
  return 0;

}

static int test_doesnt_replace_if_score_isnt_high_enough() {
  VARZIntSampleSet_t sample_set;
  VARZIntSampleSetInit(&sample_set, 3);
  VARZIntSampleAddSample(&sample_set, 101, 1, 90);
  VARZIntSampleAddSample(&sample_set, 102, 2, 90);
  VARZIntSampleAddSample(&sample_set, 103, 3, 90);

  // At this point, we should have a 75% chance (3/4) of replacing
  // So a value that's 1/4th - 1 of MAX_UINT32 should not result in replacement
  // We'll pick a value to replace with the victim being slot 2, but it shouldn't be replaced
  VARZIntSampleAddSample(&sample_set, 104, 4, makeUINT64((uint64_t) UINT32_MAX / 4-1, 2));
  
  if (sample_set.samples_size != 3) {
    return 1;
  } else if (sample_set.num_events != 4) {
    printf("Error test_doesnt_replace_if_score_isnt_high_enough, num_events should be 4, got %lu\n",
           sample_set.num_events);
    return 1;
  } else if (!samplesEqual(sample_set.samples[0], makeSample(101, 1))) {
    return 1;
  } else if(!samplesEqual(sample_set.samples[1], makeSample(102, 2))) {
    return 1;
  } else if(!samplesEqual(sample_set.samples[2], makeSample(103, 3))) {
    printf("Error test_doesnt_replace_if_score_isnt_high_enough, value at [2] should be (103, 3), " 
           " got (%lu,%lu)\n", sample_set.samples[2].sample_time, sample_set.samples[2].sample_value);
    return 1;
  }
  VARZIntSampleSetFree(&sample_set);
  return 0;
}

static int test_clear_works() {
  VARZIntSampleSet_t sample_set;
  VARZIntSampleSetInit(&sample_set, 3);
  VARZIntSampleAddSample(&sample_set, 101, 1, 90);
  VARZIntSampleSetClear(&sample_set);
  if (sample_set.samples_size != 3) {
    return 1;
  } else if (sample_set.num_events != 0) {
    printf("Error test_clear_works, num_events should be 0, got %lu\n",
           sample_set.num_events);
    return 1;
  }
  VARZIntSampleSetFree(&sample_set);
  return 0;
}


/***** TESTS FOR MHTInt Sample Variables *****/


static int test_mht_add_adds_to_both_variables() {
  uint64_t rvs[2];
  VARZMHTIntSampler_t sampler;
  VARZMHTIntSamplerInit(&sampler, 11, 4);

  // RV's shouldn't matter since we'll just shove it into both anyway
  VARZMHTIntSamplerAddSample(&sampler, 101, 1, make2UINT64s(0, 0, 0, 0, rvs));

  if (sampler.latest_time != 101) {
    printf("Error test_mht_add_adds_to_both_variables, latest_time should be 101, got %lu\n",
           sampler.latest_time);
    return 1;
  } else if (sampler.min_samples.num_events != 1) {
    printf("Error test_mht_add_adds_to_both_variables, min_samples.num_events should = 1, got %lu\n",
           sampler.min_samples.num_events);
    return 1;
  } else if(sampler.min_samples.samples_size != 4) {
    return 1;
  } else if(!samplesEqual(sampler.min_samples.samples[0], makeSample(101, 1))) {
    VARZIntSample_t sample = sampler.min_samples.samples[0];
    printf("Error test_mht_add_adds_to_both_variables, sampler.min_samples.samples[0] should be "
           "(101, 1), got (%lu, %lu)\n", sample.sample_time, sample.sample_value);
    return 1;
  } else if (sampler.all_time_samples.num_events != 1) {
    return 1;
  } else if (!samplesEqual(sampler.all_time_samples.samples[0], makeSample(101, 1))) {
    return 1;
  }

  VARZMHTIntSamplerFree(&sampler);
  return 0;
}


static int test_mht_adds_to_new_minute_cause_recycling() {
  uint64_t rvs[2];
  VARZMHTIntSampler_t sampler;
  VARZMHTIntSamplerInit(&sampler, 11, 4);

  // RV's shouldn't matter since we'll just shove it into both anyway
  VARZMHTIntSamplerAddSample(&sampler, 59, 1, make2UINT64s(0, 0, 0, 0, rvs));
  VARZMHTIntSamplerAddSample(&sampler, 61, 2, make2UINT64s(0, 0, 0, 0, rvs));
  
  if (sampler.latest_time != 61) {
    return 1;
  } else if (sampler.min_samples.num_events != 1) {
    return 1;
  } else if (!samplesEqual(sampler.min_samples.samples[0], makeSample(61, 2))) {
    return 1;
  } else if (sampler.all_time_samples.num_events != 2) {
    return 1;
  } else if (!samplesEqual(sampler.all_time_samples.samples[0], makeSample(59, 1))) {
    return 1;
  } else if (!samplesEqual(sampler.all_time_samples.samples[1], makeSample(61, 2))) {
    return 1;
  }


  VARZMHTIntSamplerFree(&sampler);
  return 0;
}


static int test_mht_json_repr() {
  uint64_t rvs[2];
  sds repr_sds;
  VARZMHTIntSampler_t sampler;
  char *desired_json_repr;
  VARZMHTIntSamplerInit(&sampler, 11, 4);

  // RV's shouldn't matter since we'll just shove it into both anyway
  VARZMHTIntSamplerAddSample(&sampler, 59, 1, make2UINT64s(0, 0, 0, 0, rvs));
  VARZMHTIntSamplerAddSample(&sampler, 61, 2, make2UINT64s(0, 0, 0, 0, rvs));

  repr_sds = sdsempty();
  VARZMHTIntSamplerJSONRepr(&sampler, &repr_sds);

  desired_json_repr = "{\"latest_time_sec\":61,"
      "\"last_minute_samples\":"
        "{\"sample_values\":[2],\"sample_times_sec\":[61],\"samples_size\":4,\"num_events\":1},"
      "\"all_time_samples\":"
        "{\"sample_values\":[1,2],\"sample_times_sec\":[59,61],\"samples_size\":4,\"num_events\":2}}";

  if (strcmp(desired_json_repr, repr_sds)) {
    printf("ERROR: test_mht_json_repr strings don't match\n");
    printf("GOT: %s\n", repr_sds);
    return 1;
  }

  VARZMHTIntSamplerFree(&sampler);
  sdsfree(repr_sds);
  return 0;

}


int sample_variable_tests() {
  int failure_count = 0;

  // For int
  failure_count += test_int_sample_adds_until_full_without_replacement();
  failure_count += test_does_replacement_if_score_is_high_enough();
  failure_count += test_doesnt_replace_if_score_isnt_high_enough();
  failure_count += test_clear_works();

  // For MHT
  failure_count += test_mht_add_adds_to_both_variables();
  failure_count += test_mht_adds_to_new_minute_cause_recycling();
  failure_count += test_mht_json_repr();
  return failure_count;
}
