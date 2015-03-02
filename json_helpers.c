#include <string.h>

#include "json_helpers.h"

/***** STATIC HELPER PROTOTYPES *****/
static void sdscatprintf_ptr(sds *s_ptr, const char *fmt, ...);
static void append_escaped_json_string(sds *s_ptr, char *str);
static int add_backslash_escaped_to_buf(char *buf, char c);

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
  //*dest = sdscatrepr(*dest, unquoted_name, strlen(unquoted_name));
  append_escaped_json_string(dest, unquoted_name);
  *dest = sdscat(*dest, ":");
}


void VARZJSONStringRepr(sds *dest, char *s) {
  // TODO: This probably isn't the right function to use, let's make our own for JSON
  //*dest = sdscatrepr(*dest, s, strlen(s));
  append_escaped_json_string(dest, s);
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

static void append_escaped_json_string(sds *dest, char *str) {
  int str_len, buf_len, buf_idx;
  str_len = strlen(str);

  // The most we need is leading and trailing quotes, \0 and 2x for the chars
  buf_len = str_len * 2 + 3;

  char buf[buf_len];
  buf_idx = 0;

  buf[buf_idx] = '"';
  buf_idx ++;

  for (int i=0; i < str_len; i++) {
    char c = str[i];
    if (c == ' ' || c == '!' || (c >= '#' && c <= '~')) {
      buf[buf_idx] = c;
      buf_idx ++;
    } else {
      switch (c) {
        case '"':
          buf_idx += add_backslash_escaped_to_buf(buf+buf_idx, '"');
          break;
        case '\\':
          buf_idx += add_backslash_escaped_to_buf(buf+buf_idx, '\\');
          break;
        case '\b':
          buf_idx += add_backslash_escaped_to_buf(buf+buf_idx, 'b');
          break;
        case '\f':
          buf_idx += add_backslash_escaped_to_buf(buf+buf_idx, 'f');
          break;
        case '\n':
          buf_idx += add_backslash_escaped_to_buf(buf+buf_idx, 'n');
          break;
        case '\r':
          buf_idx += add_backslash_escaped_to_buf(buf+buf_idx, 'r');
          break;
        case '\t':
          buf_idx += add_backslash_escaped_to_buf(buf+buf_idx, 't');
          break;
        //TODO: Deal with character sequences; for now we'll just ignore them
      }
    }
  }
  //Trailing quote
  buf[buf_idx] = '"';
  buf_idx ++;
  // Escape
  buf[buf_idx] = '\0';

  *dest = sdscat(*dest, buf);
}


// Add the two character sequence to the buffer of backslash followed by the character
// ex: add_backslash_escaped_to_buf(buf, 'b') adds "\b" to the buffer
static int add_backslash_escaped_to_buf(char *buf, char c) {
  buf[0] = '\\';
  buf[1] = c;
  return 2;
}
