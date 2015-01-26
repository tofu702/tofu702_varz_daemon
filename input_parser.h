#ifndef INPUT_PARSER_H_
#define INPUT_PARSER_H_

#include "command_description.h"


#define VARZ_MAX_OP_LEN 128
#define VARZ_MHT_COUNTER_ADD_OP_NAME "MHTCOUNTERADD"
#define VARZ_MHT_SAMPLE_ADD_OP_NAME "MHTSAMPLEADD"


struct VARZOperationDescription VARZOpCmdParse(char *cmd);


#endif