#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdint.h>

void VARZRandReseed();
uint64_t VARZRand64();

#ifdef VARZ_STUB

/* If VARZ_STUB is set (usually via gcc) this allows us to set the output of VARZRand64.
 * Provide an array of values that should be returned with the calls to VARZRand64 in order.
 * Values will be copied and dynamically allocated. Subsequent calls to this function will
 * free the prior sets of values.
 */ 
void VARZRand64StubValues(uint64_t *values, unsigned int num_values);

#endif

#endif 
