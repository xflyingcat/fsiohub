/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "includes.h"

#define SERIAL_PROTO_BUF_SIZE 200

typedef struct {
char buf[SERIAL_PROTO_BUF_SIZE];
int cnt;
int len;
int state;
} SERIAL_PROTO_STRUCT;


typedef struct {
  unsigned char event_type;
  unsigned char port_id;
  unsigned short event_id;
  long value;
} EVENT_STRUCT;


DWORD WINAPI serial_thread(LPVOID parms);
DWORD WINAPI supervisor_thread(LPVOID parms);
void start_serial(SESSION_WORK_SPACE *sws);
void serial_stop(SESSION_WORK_SPACE *sws);
void hexdump(void *adr,int len);
int serial_open(SESSION_WORK_SPACE *sws, const char *dev_name, long baud_rate);
void serial_close(SESSION_WORK_SPACE *sws);
void on_serial_data(unsigned char *recvbuf,int len);
int serial_reading_thread(LPVOID lpParameter);
void on_serial_timeout(SESSION_WORK_SPACE *sws);
int serial_write(SESSION_WORK_SPACE *sws, void *ibuf, int len);
int serial_read(SESSION_WORK_SPACE *sws, unsigned char *buf,int len);
void serial_signal_to_close(SESSION_WORK_SPACE *sws);
void set_flag_opened(SESSION_WORK_SPACE *sws);
int is_opened(SESSION_WORK_SPACE *sws);


void pipes_destroy(void);
void pipes_create(void);
void flush_from_sim(void);
void flush_from_device(void);
int send_to_sim(ULONG64 data);
int recv_from_device(ULONG64 *data);
int recv_from_sim(ULONG64 *data);
int send_to_device(ULONG64 data);
void set_pipes_off(void);
void set_pipes_on(void);
int ports_detect(void);
#endif

