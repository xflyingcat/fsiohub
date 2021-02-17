#ifndef __FSUIPC_UTILS_H__
#define __FSUIPC_UTILS_H__


typedef struct {
int id;
int offset;
int size;
const char *io_code;
long val;
} FS_VAR_EVENT;

int fsuipc_connect(void);
int read_single_fs_var(int offset,int size,void *val);
int read_fs_var(FS_VAR_EVENT *fs_event_vars, char *io_code);
int write_fs_var(FS_VAR_EVENT *fs_event_vars, int idx);
int write_single_fs_var(int offset,int size,void *val);

unsigned long bcd2bin(unsigned long bcd);
unsigned short bin2bcd(unsigned short bin);
unsigned long long2bcd (unsigned int val);
short comfreq_100khz_recovery(unsigned long val);
unsigned long ulong_hex(const char *in);
int hex2int(char *hh);
void reset_fsuipc_flag(void);
void set_fsuipc_flag(void);
int get_fsuipc_flag(void);
int event2hex(const char *in);
#endif

