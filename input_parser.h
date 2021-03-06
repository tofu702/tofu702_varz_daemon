#ifndef INPUT_PARSER_H_
#define INPUT_PARSER_H_

#include "command_description.h"


#define VARZ_MAX_OP_LEN 128
#define VARZ_MHT_COUNTER_ADD_OP_NAME "MHTCOUNTERADD"
#define VARZ_MHT_COUNTER_GET_OP_NAME "MHTCOUNTERGET"
#define VARZ_MHT_SAMPLE_ADD_OP_NAME "MHTSAMPLEADD"
#define VARZ_ALL_DUMP_JSON_OP_NAME "ALLDUMPJSON"
#define VARZ_ALL_LIST_JSON_OP_NAME "ALLLISTJSON"
#define VARZ_ALL_FLUSH_OP_NAME "ALLFLUSH"


struct VARZOperationDescription VARZOpCmdParse(char *cmd, int cmd_len);


#endif
