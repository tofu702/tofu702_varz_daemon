#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "all_tests.h"
#include "hash_table.h"

static int test_add_and_get_unique_slots() {
  char dummy1[3], dummy2[3];
  VARZHashTable_t ht;

  VARZHashTableInit(&ht, 4);
  VARZHashTableAdd(&ht, "foo", 1, dummy1);
  VARZHashTableAdd(&ht, "bar", 2, dummy2);
  if (ht.num_slots != 4) {
    printf("ERROR test_add_and_get_unique_slots, num_slots should be 4, got %d\n", ht.num_slots);
    return 1;
  } else if(ht.total_entries != 2) {
    printf("ERROR test_add_and_get_unique_slots, total_entries should be 2, got %u\n",
        ht.total_entries);
    return 1;
  } else if(VARZHashTableGet(&ht, "foo", 1) != (void*) dummy1) {
    return 1;
  } else if(VARZHashTableGet(&ht, "bar", 2) != (void*) dummy2) {
    return 1;
  } else if (ht.slots[0].num_entries != 0) {
    return 1;
  } else if (ht.slots[0].entries != NULL) {
    return 1;
  } else if(ht.slots[1].num_entries != 1) {
    return 1;
  } else if(ht.slots[1].entries[0].value != (void*) dummy1) {
    return 1;
  }
  
  VARZHashTableFree(&ht);
  return 0;
}

static int test_get_for_non_existant_value_returns_null() {
  char dummy1[3], dummy2[3];
  VARZHashTable_t ht;

  VARZHashTableInit(&ht, 4);
  VARZHashTableAdd(&ht, "foo", 1, dummy1);
  VARZHashTableAdd(&ht, "bar", 2, dummy2);

  if (VARZHashTableGet(&ht, "zork", 3) != NULL) {
    return 1;
  }

  VARZHashTableFree(&ht);
  return 0;
}

static int test_add_and_get_resolve_for_multiple_items_in_same_slot() {
  char dummy1[3], dummy2[3];
  VARZHashTable_t ht;

  VARZHashTableInit(&ht, 4);
  VARZHashTableAdd(&ht, "foo", 1, dummy1);
  VARZHashTableAdd(&ht, "bar", 1+4, dummy2);

  if(VARZHashTableGet(&ht, "foo", 1) != (void*) dummy1) {
    printf("ERROR test_add_and_get_resolve_for_multiple_items_in_same_slot, unable to get \"foo\""
           "from ht, should be %p, got %p\n", dummy1, VARZHashTableGet(&ht, "foo", 1));
    return 1;
  } else if(VARZHashTableGet(&ht, "bar", 1+4) != (void*) dummy2) {
    printf("ERROR test_add_and_get_resolve_for_multiple_items_in_same_slot, unable to get \"bar\""
           "from ht, should be %p, got %p\n", dummy2, VARZHashTableGet(&ht, "bar", 1+4));
    return 1;
  } else if(ht.total_entries != 2) {
    return 1;
  } else if(ht.slots[1].num_entries != 2) {
    return 1;
  }

  VARZHashTableFree(&ht);
  return 0;
}

static int test_add_and_get_resolve_for_same_hash_different_strings() {
  char dummy1[3], dummy2[3];
  VARZHashTable_t ht;

  VARZHashTableInit(&ht, 4);
  VARZHashTableAdd(&ht, "foo", 1, dummy1);
  VARZHashTableAdd(&ht, "bar", 1, dummy2);

  if(VARZHashTableGet(&ht, "foo", 1) != (void*) dummy1) {
    printf("ERROR test_add_and_get_resolve_for_same_hash_different_strings, unable to get \"foo\""
           "from ht, should be %p, got %p\n", dummy1, VARZHashTableGet(&ht, "foo", 1));
    return 1;
  } else if(VARZHashTableGet(&ht, "bar", 1) != (void*) dummy2) {
    printf("ERROR test_add_and_get_resolve_for_same_hash_different_strings, unable to get \"bar\""
           "from ht, should be %p, got %p\n", dummy2, VARZHashTableGet(&ht, "bar", 1+4));
    return 1;
  } else if(ht.total_entries != 2) {
    return 1;
  } else if(ht.slots[1].num_entries != 2) {
    return 1;
  }

  VARZHashTableFree(&ht);
  return 0;
}


static void visit_test_visitor(struct VARZHashTableEntry *entry, void *results) {
  void **results_arr = (void**) results;
  int *counter = (int *)results_arr[0];
  char **values = (char **)results_arr[1];
  values[*counter] = entry->value;
  (*counter) ++;
}

static bool arr_contains(char **arr, int arr_len, char *ptr) {
  for (int i=0; i < arr_len; i++) {
    if (arr[i] == ptr) {
      return true;
    }
  }
  return false;
}


static int test_visit() {
  char dummy1[3], dummy2[3], dummy3[3];
  int count = 0;
  char *values[3];
  void *results[2];
  results[0] = &count;
  results[1] = values;
  VARZHashTable_t ht;

  VARZHashTableInit(&ht, 4);
  VARZHashTableAdd(&ht, "foo", 1, dummy1);
  VARZHashTableAdd(&ht, "bar", 1, dummy2);
  VARZHashTableAdd(&ht, "baz", 2, dummy3);

  VARZHashTableVisit(&ht, &visit_test_visitor, results);
  if (count != 3) {
    return 1;
  } else if (!arr_contains(values, 3, dummy1)) {
    return 1;
  } else if (!arr_contains(values, 3, dummy2)) {
    return 1;
  } else if (!arr_contains(values, 3, dummy3)) {
    return 1;
  }
  VARZHashTableFree(&ht);
  return 0;
}


int hash_table_tests() {
  int failure_count = 0;
  
  failure_count += test_add_and_get_unique_slots();
  failure_count += test_get_for_non_existant_value_returns_null();
  failure_count += test_add_and_get_resolve_for_multiple_items_in_same_slot();
  failure_count += test_add_and_get_resolve_for_same_hash_different_strings();
  failure_count += test_visit();

  return failure_count;
}
