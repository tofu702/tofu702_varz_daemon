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

struct VARZOperationDescription VARZOpCmdParse(char *cmd) {
  struct VARZOperationDescription desc;
  char op[VARZ_MAX_OP_LEN];
  int op_len, var_name_len;

  // Parse out the op
  op_len = getNextWord(cmd, op, VARZ_MAX_OP_LEN);
  if (op_len < 0) {
    desc.op = VARZOP_INVALID;
    return desc;
  }
  desc.op = opNameToType(op);

  // Parse out the name
  char *var_name_pos = cmd + op_len + 1;
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
    default:
      desc.op = VARZOP_INVALID;
  }
  return desc;
}


/***** STATIC HELPERS *****/

static enum VARZOperationType opNameToType(char *name) {
  if (!strcmp(VARZ_MHT_COUNTER_ADD_OP_NAME, name)) {
    return VARZOP_MHT_COUNTER_ADD;
  } else if(!strcmp(VARZ_MHT_SAMPLE_ADD_OP_NAME, name)) {
    return VARZOP_MHT_SAMPLE_ADD;
  } else {
    return VARZOP_INVALID;
  }
}

// Read the input up to the next space and copy the contents into dest.
// Returns the number of characters read
// If we don't find a space in the next dest_len, return -1 and don't populate dest
static int getNextWord(char *in_string, char *dest, int dest_len) {
  char *loc;
  int word_len;
  loc = strchr(in_string, ' ');
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
