/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/
#define  PLUGIN_REVISION  "0.7.2"

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#define BAUD_RATE 115200

#define IBM 1

#define SERIAL_PROTO_BUF_SIZE 200

#define SERIAL_PORTS_MAX  8


#include <process.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <winbase.h>


#ifndef CODEBLOCKS

#endif

#define STX '{'
#define ETX '}'



typedef struct 	{
  HANDLE serial_thread_handle;
  HANDLE supervisor_thread_handle;
  HANDLE serial_handle;
  SOCKADDR* client_addr;
  int client_addr_size;
  long live_counter;
  int serial_connect_flag;
  int opened;
  int error_flag;
  char serial_port[80];
  int port_id;
  int disp_cnt;
  int to;
  int numeric_outs_max;
  int discrete_outs_max;
} SESSION_WORK_SPACE;


typedef struct {
int id;
int ref;
const char *dref_name;
int var_type;
int acc_type;
float fval;
int ival;
} DATA_REF_TABLE;


typedef struct {
int id;
int ref;
const char *cref_name;
int cmd_type;
int ll_iocode;
}  CMD_REF_TABLE;


enum {
  DEC = 0,
  BCD,
  HEX
};

enum {
  MODE_NOP = 0,
  MODE_DISCRETE_EVENT,
  MODE_DISCRETE_LOOP_EVENT,
  MODE_DISCRETE_LIMIT_EVENT,
  MODE_TOGGLE_EVENT,
  MODE_NUMERIC_EVENT,
  MODE_DISCRETE_POLLING,
  MODE_NUMERIC_POLLING
};

enum {
  TYPE_INT = 0,
  TYPE_UINT,
  TYPE_FLOAT,
  TYPE_STRING,
};

enum {
   SECTION_INPUT = 0,
   SECTION_OUTPUT,
   SECTION_INPUT_AXIS
};


typedef struct {
  const char* key;
  int val;
} DICT;



typedef struct {
int key_id;
int offset;
int size;
int type;
int mode;
float min_float;
float max_float;
float inc_float;
float state_float;
int min_int;
int max_int;
int inc_int;
int port;
int encoder_int[2];
float encoder_float[2];
int index;
int devadr;
int pos;
int len;
float factor;
float zero_offset;
int opt;
int encoding;
int control;
unsigned long mask;
} KEY_CONFIG;

#define MAX_KEYS  (256*SERIAL_PORTS_MAX)
extern KEY_CONFIG key_config[MAX_KEYS];
extern int output_data[MAX_KEYS];
extern int input_data[MAX_KEYS];
void poll_inputs(void);


#include "fsuipc_utils.h"
#include "misc_utils.h"
#include "ini.h"
#include "config.h"
#include "serial.h"
#include "logger.h"
#include "devside.h"
#include "simside.h"
#include "console_utils.h"

extern SESSION_WORK_SPACE sws[SERIAL_PORTS_MAX];
void print_config_line(KEY_CONFIG *cfg_item);
#endif

