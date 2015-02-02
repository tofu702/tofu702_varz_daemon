#ifndef SAMPLE_VARIABLE_H_
#define SAMPLE_VARIABLE_H_

#include <stdint.h>

#include "sds/sds.h"

#include "time_utils.h"

typedef struct {
  unsigned long sample_value; 
  varz_time_t sample_time;
} VARZIntSample_t ;

typedef struct {
  VARZIntSample_t *samples;
  unsigned long samples_size;
  unsigned long num_events;
} VARZIntSampleSet_t;

typedef struct {
  // We may want to handle hour some other way
  VARZIntSampleSet_t min_samples, all_time_samples;
  varz_time_t latest_time;
} VARZMHTIntSampler_t;

/***** FOR VARZMHTIntSample *****/

/* Allocate all memory associated with VarzMHTIntSample struct, but not the struct itself. 
 * Arguments:
 * -sampler: A pointer to a valid VARZMHTIntSampler_t (can be uninitialized)
 * -start_time: The starting point from which samples should be recorded
 * -samples_per_set: For such sub set of sample, how big should the set be. Recommended: 1000
 */ 
void VARZMHTIntSamplerInit(VARZMHTIntSampler_t *sampler, varz_time_t start_time, 
                           unsigned int samples_per_set);

void VARZMHTIntSamplerFree(VARZMHTIntSampler_t *sampler);
void VARZMHTIntSamplerAddSample(VARZMHTIntSampler_t *sample, varz_time_t sample_time,
    unsigned long sample_value, uint64_t random_vals[2]);
void VARZMHTIntSamplerJSONRepr(VARZMHTIntSampler_t *sampler, sds *dest);



/***** FOR VARZIntSampleSet_t *****/

/* Initialize the memory at the provided VARZIntSampleSet_t. 
 * Arguments:
 * -sample_set: A pointer to an existing (allocated or stack) VARZIntSampleSet_t, but without
 *    a samples array.
 * -samples_size: The number of samples that will be stored in the sample_set. This number will
 *    be the maximum number of samples we save, but we may have less under certain conditions.
 */ 
void VARZIntSampleSetInit(VARZIntSampleSet_t *sample_set, unsigned long samples_size);

/* Restore a sample set to a state as if it had no samples in it.
 * NOTE: Re-using an existing sample set is much faster than freeing and re-initting
 * Arguments:
 * -sample_set: A pointer to a valid VARZIntSampleSet_t
 */
void VARZIntSampleSetClear(VARZIntSampleSet_t *sample_set);

// Doesn't free the sample_set ptr itself because it could be allocated on the stack
void VARZIntSampleSetFree(VARZIntSampleSet_t *sample_set);
void VARZIntSampleAddSample(VARZIntSampleSet_t *sample_set, varz_time_t sample_time,
    unsigned long sample_value, uint64_t random_value);
void VARZIntSampleSetJSONRepr(VARZIntSampleSet_t *sample_set, sds *dest);

#endif

