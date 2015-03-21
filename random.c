#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "random.h"

/***** STANDARD IMPLEMENTATION *****/

#ifndef VARZ_STUB

void VARZRandReseed() {
  FILE *infile;
  unsigned seed;
  infile = fopen("/dev/random", "rb");
  assert(infile);
  assert(fread(&seed, 1, sizeof(seed), infile) == 1);
  fclose(infile);
  srand(seed);
}


uint64_t VARZRand64() {
  return (((uint64_t) rand()) << 34) | (((uint64_t) rand()) << 2) | ((uint64_t) rand() % 4);
}

#endif

/***** STUB IMPLEMENTATION *****/

#ifdef VARZ_STUB

// Globals for random stub
uint64_t *varz_rand64_stub_values = NULL;
unsigned int varz_rand64_stub_num_values = 0;
unsigned int varz_rand64_stub_cur_idx = 0;

void VARZRandReseed() {
  return; // NOOP
}

uint64_t VARZRand64() {
  assert(varz_rand64_stub_cur_idx < varz_rand64_stub_num_values);
  return varz_rand64_stub_values[varz_rand64_stub_cur_idx++];
}

void VARZRand64StubValues(uint64_t *values, unsigned int num_values) {
  if (varz_rand64_stub_values) {
    free(varz_rand64_stub_values);
  }

  varz_rand64_stub_values = calloc(num_values, sizeof(uint64_t));
  memcpy(varz_rand64_stub_values, values, sizeof(uint64_t) * num_values);
  varz_rand64_stub_num_values = num_values;
  varz_rand64_stub_cur_idx = 0;
}

#endif
