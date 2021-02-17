/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/
#ifndef __PROTOUTIL_H__
#define __PROTOUTIL_H__

char pLogFile[MAX_PATH];

int add_float_ieee754(unsigned char *wp,float val);
int asm_float_ieee754(unsigned char *wp, float *val);
int add_int32(unsigned char *wp,long val);
int asm_int32(unsigned char *wp,long *val);
int add_int16(unsigned char *wp,int val);
int asm_int16(unsigned char *wp,unsigned short *val);
int add_int8(unsigned char *wp,int val);
int asm_int8(unsigned char *wp,int *val);
int make_buffer_from_line(char *in, unsigned char *out);
void line_input(const char *prompt, char *buf);
void hexdump(void *adr,int len);
int add_symbol(unsigned char *wp,int val);
int add_hex_byte(unsigned char *wp,int val);
int lrc_calc(unsigned char *buf,int len);
unsigned short unsigned_short_from_hex(char *in);
unsigned long ulong_hex(const char *in);
int event2hex(const char *in);
#endif


