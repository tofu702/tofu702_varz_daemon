/***** NOTE *****
 * This hash table is specifically designed as a "get optimized" hash table. Add operations
 * may be a bit more expensive, but Get operations should be cheaper. Specially this is because
 * collision are resolved using an array of entries in that bucket (instead of a linked list). 
 * This array is grown by using realloc.
 */ 

#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <stdint.h>

#define VARZ_HASHTABLE_MAX_NAME_LEN 128

struct VARZHashTableEntry {
  uint64_t name_hash;
  char name[VARZ_HASHTABLE_MAX_NAME_LEN];
  void *value;
};


struct VARZHashTableSlot {
  struct VARZHashTableEntry *entries;
  unsigned int num_entries;
};


typedef struct {
  struct VARZHashTableSlot *slots;
  unsigned int num_slots;
  unsigned int total_entries;
} VARZHashTable_t; 


void VARZHashTableInit(VARZHashTable_t *ht, unsigned int num_slots); 

void VARZHashTableFree(VARZHashTable_t *ht); 

void VARZHashTableAdd(VARZHashTable_t *ht, char name[128], uint64_t name_hash, void *value);


// Return NULL if entry not found
void *VARZHashTableGet(VARZHashTable_t *ht, char name[128], uint64_t name_hash);


#endif
