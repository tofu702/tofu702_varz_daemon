#ifndef JSON_HELPERS_H_
#define JSON_HELPERS_H_


#include "sds/sds.h"

#include "time_utils.h"


// These are intentionally quite conservative
#define JSON_BYTES_NEEDED_FOR_UL 32
#define JSON_BYTES_NEEDED_FOR_TIME JSON_BYTES_NEEDED_FOR_UL


void VARZJSONUnsignedLongArrToRepr(sds *dest, unsigned long *arr, size_t num_elem);


void VARZJSONArrayStart(sds *dest);
void VARZJSONArrayNextItem(sds *dest);
void VARZJSONArrayEnd(sds *dest);


void VARZJSONDictStart(sds *dest);
void VARZJSONDictNextKey(sds *dest);
void VARZJSONDictEnd(sds *dest);

// Includes the trailing ':'
void VARZJSONDictKey(sds *dest, char *unquoted_name);

void VARZJSONStringRepr(sds *dest, char *s);
void VARZJSONUnsignedLongRepr(sds *dest, unsigned long l);
void VARZJSONTimeRepr(sds *dest, varz_time_t time);



#endif
