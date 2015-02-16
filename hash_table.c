#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "hash_table.h"


/***** STATIC HELPER PROTOTYPES *****/
static unsigned int computeSlot(uint64_t hash_value, unsigned int num_slots);

/***** INTERFACE IMPLEMENTATION *****/

void VARZHashTableInit(VARZHashTable_t *ht, unsigned int num_slots) {
  bzero(ht, sizeof(VARZHashTable_t));
  ht->num_slots = num_slots;
  ht->slots = calloc(num_slots, sizeof(struct VARZHashTableSlot));
}


void VARZHashTableFree(VARZHashTable_t *ht) {
  for(int i=0; i < ht->num_slots; i++) {
    free(ht->slots[i].entries); // Could be NULL, but free is okay with that
  }

  free(ht->slots);

  //Not strictly needed but should throw more obvious errors if accessed illegally
  bzero(ht, sizeof(VARZHashTable_t));
}


void VARZHashTableAdd(VARZHashTable_t *ht, char name[128], uint64_t name_hash, void *value) {
  struct VARZHashTableSlot *slot;
  struct VARZHashTableEntry *entry;

  // We shouldn't add an entry we already have
  assert(VARZHashTableGet(ht, name, name_hash) == NULL);

  slot = ht->slots + computeSlot(name_hash, ht->num_slots);

  // slot->entries could be NULL, but realloc is ok with that
  slot->entries = realloc(slot->entries, (slot->num_entries+1) * sizeof(struct VARZHashTableEntry));
  slot->num_entries ++;
  entry = slot->entries + (slot->num_entries-1);
  entry->name_hash = name_hash;
  strncpy(entry->name, name, VARZ_HASHTABLE_MAX_NAME_LEN);
  entry->name[VARZ_HASHTABLE_MAX_NAME_LEN-1] = '\0'; // Strncpy doesn't always terminate the string
  entry->value = value;

  ht->total_entries ++;
}


void *VARZHashTableGet(VARZHashTable_t *ht, char name[128], uint64_t name_hash) {
  struct VARZHashTableSlot *slot;
  struct VARZHashTableEntry *entries;
  unsigned int num_entries;

  slot = ht->slots + computeSlot(name_hash, ht->num_slots);

  // Cache for performance
  num_entries = slot->num_entries;
  entries = slot->entries;

  // TODO: Performance Optimization: Sort the slots
  for (int i=0; i < num_entries; i++) {
    if(entries[i].name_hash == name_hash && 
       !strncmp(name, entries[i].name, VARZ_HASHTABLE_MAX_NAME_LEN-1)) {
      return entries[i].value;
    }
  }

  return NULL;
}


void VARZHashTableVisit(VARZHashTable_t *ht, VARZHashTableVistor visitor, void *pass_through_data) {
  for (unsigned int i=0; i < ht->num_slots; i++) {
    struct VARZHashTableSlot *slot = ht->slots + i;
    for (unsigned int j=0; j < slot->num_entries; j++) {
      struct VARZHashTableEntry *entry = slot->entries + j;
      visitor(entry, pass_through_data);
    }
  }
}


/***** STATIC HELPERS *****/
static unsigned int computeSlot(uint64_t hash_value, unsigned int num_slots) {
  return hash_value % num_slots;
}
