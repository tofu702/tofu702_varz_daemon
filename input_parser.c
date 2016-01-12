#include <stdio.h>
#include <string.h>

#include "input_parser.h"
#include "random.h"


/***** STATIC HELPER PROTOTYPES *****/
static enum VARZOperationType opNameToType(char *name);
static int getNextWord(char *in_string, char *dest, int dest_len);
void MHTCounterParse(char *cmd_remainder, struct VARZOperationDescription *dest);
void MHTSampleParse(char *cmd_remainder, struct VARZOperationDescription *dest);


/***** INTERFACE IMPLEMENTATION *****/

struct VARZOperationDescription VARZOpCmdParse(char *cmd, int cmd_len) {
  // TODO: This whole function is sorta ugly & hackish, refactor it into something nicer
  struct VARZOperationDescription desc;
  char cmd_copy[cmd_len+1], op[VARZ_MAX_OP_LEN];
  int op_len, var_name_len;

  // Copy (We could potentially do this destructively instead)
  strncpy(cmd_copy, cmd, cmd_len);
  cmd_copy[cmd_len] = '\0';

  // Check for trailing ';'
  if (cmd_copy[cmd_len-1] != ';') {
    desc.op = VARZOP_INVALID;
    return desc;
  }

  // Otherwise it will be combined with the last string element
  cmd_copy[cmd_len-1] = '\0';

  // Parse out the op
  op_len = getNextWord(cmd_copy, op, VARZ_MAX_OP_LEN);
  if (op_len < 0) {
    desc.op = VARZOP_INVALID;
    return desc;
  }
  desc.op = opNameToType(op);

  // Parse out the name
  char *var_name_pos = cmd_copy + op_len + 1;
  var_name_len = getNextWord(var_name_pos, desc.variable_name, VARZ_HASHTABLE_MAX_NAME_LEN);
  if (var_name_len < 0) {
    desc.op = VARZOP_INVALID;
    return desc;
  }

  char *sub_command_pos = var_name_pos + var_name_len + 1;

  switch (desc.op) {
    case VARZOP_MHT_COUNTER_ADD:
      MHTCounterParse(sub_command_pos, &desc);
      break;
    case VARZOP_MHT_SAMPLE_ADD:
      MHTSampleParse(sub_command_pos, &desc);
      break;
    case VARZOP_ALL_DUMP_JSON:
      break;
    case VARZOP_ALL_LIST_JSON:
      break;
    case VARZOP_ALL_FLUSH:
      break;
    case VARZOP_MHT_COUNTER_GET:
      break;
    default:
      desc.op = VARZOP_INVALID;
  }
  return desc;
}


/***** STATIC HELPERS *****/

static enum VARZOperationType opNameToType(char *name) {
  if (!strcmp(VARZ_MHT_COUNTER_ADD_OP_NAME, name)) {
    return VARZOP_MHT_COUNTER_ADD;
  } else if (!strcmp(VARZ_MHT_COUNTER_GET_OP_NAME, name)) {
    return VARZOP_MHT_COUNTER_GET;
  } else if(!strcmp(VARZ_MHT_SAMPLE_ADD_OP_NAME, name)) {
    return VARZOP_MHT_SAMPLE_ADD;
  } else if(!strcmp(VARZ_ALL_DUMP_JSON_OP_NAME, name)) {
    return VARZOP_ALL_DUMP_JSON;
  } else if(!strcmp(VARZ_ALL_LIST_JSON_OP_NAME, name)) {
    return VARZOP_ALL_LIST_JSON;
  } else if(!strcmp(VARZ_ALL_FLUSH_OP_NAME, name)) {
    return VARZOP_ALL_FLUSH;
  } else {
    return VARZOP_INVALID;
  }
}

// Read the input up to the next space and copy the contents into dest.
// Returns the number of characters read
// If we don't find a space copies the remaining characters in the string
static int getNextWord(char *in_string, char *dest, int dest_len) {
  char *loc;
  int word_len;
  loc = strchr(in_string, ' ');

  if (!loc) {
    loc = in_string + strlen(in_string);
  }

  if (!loc) {
    dest[0] = '\0';
    return -1;
  }
  word_len = (loc - in_string);
  if(word_len > dest_len) {
    dest[0] = '\0';
    return -1;
  }

  strncpy(dest, in_string, word_len);
  dest[word_len] = '\0';

  return word_len;
}

void MHTCounterParse(char *cmd_remainder, struct VARZOperationDescription *dest) {
  struct VARZMHTCounterAddOp *add_op = &(dest->op_data.counter_add_op);
  // TODO: Some sanity checking
  int rv = sscanf(cmd_remainder, "%lu %u", &(add_op->time), &(add_op->amt));

  if (rv != 2) {
    dest->op = VARZOP_INVALID;
  }
}


void MHTSampleParse(char *cmd_remainder, struct VARZOperationDescription *dest) {
  struct VARZMHTSamplerAddOp *sampler_op = &(dest->op_data.sampler_add_op);
  int rv = sscanf(cmd_remainder, "%lu %lu", &(sampler_op->time), &(sampler_op->value));
  if (rv != 2) {
    dest->op = VARZOP_INVALID;
    return;
  }

  sampler_op->random_vals[0] = VARZRand64();
  sampler_op->random_vals[1] = VARZRand64();
}
