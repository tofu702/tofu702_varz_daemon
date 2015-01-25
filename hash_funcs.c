#include <string.h>
#include "hash_funcs.h"


/***** STANDARD IMPLEMENTATION *****/

uint64_t VARZHashString(char *s) {
  return VARZHash((uint8_t*) s, strlen(s));
}

uint64_t VARZHash(uint8_t *bytes, unsigned int len) {
  // Implements sdbm (http://www.cse.yorku.ca/~oz/hash.html)
  uint64_t hash_val = 0;
  for(unsigned int i=0; i < len; i++) {
    hash_val = hash_val * 65599 + bytes[i];
  }

  return hash_val;
}
