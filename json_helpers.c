#include <string.h>

#include "json_helpers.h"

/***** STATIC HELPER PROTOTYPES *****/
static void sdscatprintf_ptr(sds *s_ptr, const char *fmt, ...);

/*****INTERFACE IMPLEMENTATION*****/

void VARZJSONUnsignedLongArrToRepr(sds *dest, unsigned long *arr, size_t num_elem) {
  sdscatprintf_ptr(dest, "[");
  for(int i=0; i < num_elem; i++) {
    // Don't print the leading comma the first time through
    if (i == 0) {
      sdscatprintf_ptr(dest, "%lu", arr[i]);
    } else {
      sdscatprintf_ptr(dest, ",%lu", arr[i]);
    }
  }
  sdscatprintf_ptr(dest, "]");
}

void VARZJSONArrayStart(sds *dest) {
  *dest = sdscat(*dest, "[");
}

void VARZJSONArrayNextItem(sds *dest) {
  *dest = sdscat(*dest, ",");
}

void VARZJSONArrayEnd(sds *dest) {
  *dest = sdscat(*dest, "]");
}

void VARZJSONDictStart(sds *dest) {
  *dest = sdscat(*dest, "{");
}

void VARZJSONDictNextKey(sds *dest) {
  *dest = sdscat(*dest, ",");
}

void VARZJSONDictEnd(sds *dest) {
  *dest = sdscat(*dest, "}");
}

void VARZJSONDictKey(sds *dest, char *unquoted_name) {
  // TODO: This probably isn't the right function to use, let's make our own for JSON
  *dest = sdscatrepr(*dest, unquoted_name, strlen(unquoted_name));
  *dest = sdscat(*dest, ":");
}


void VARZJSONStringRepr(sds *dest, char *s) {
  // TODO: This probably isn't the right function to use, let's make our own for JSON
  *dest = sdscatrepr(*dest, s, strlen(s));
}


void VARZJSONUnsignedLongRepr(sds *dest, unsigned long l) {
  sdscatprintf_ptr(dest, "%lu", l);
}


void VARZJSONTimeRepr(sds *dest, varz_time_t time) {
  sdscatprintf_ptr(dest, "%lu", time);
}


/*****STATIC HELPER IMPLEMENTATIONS*****/
static void sdscatprintf_ptr(sds *s_ptr, const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  *s_ptr = sdscatvprintf(*s_ptr, fmt, ap);
  va_end(ap);
}

