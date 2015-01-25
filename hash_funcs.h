#ifndef HASH_FUNCS_H_
#define HASH_FUNCS_H_

#include <stdint.h>


uint64_t VARZHash(uint8_t *bytes, unsigned int len);
uint64_t VARZHashString(char *s);



#endif
