/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#include "includes.h"


#if 0
unsigned short bcd2bin(unsigned short bcd)
{
return
       100*   ((bcd % 0x0F00) >> 8) +
        10 *  ((bcd & 0x00F0) >> 4) +
         1 *   (bcd & 0x000F);
}
#endif // 0

#if 0
unsigned short bin2bcd(unsigned short bin)
{
unsigned short result;
result = 256 *  (bin/100);
result +=  16 * ((bin%100)/10);
result +=   1 * ((bin%100)%10);
return result;
}
#endif // 0

#if 0
short comfreq_100khz_recovery(unsigned long val)
{
short tmp = val;
      tmp = (tmp << 4) & 0x0FF0;
      if(
         (tmp & 0x00F0) == 0x0020  ||
         (tmp & 0x00F0) == 0x0070
        )
          tmp |= 0x0005;
return (short)bcd2bin(tmp);
}
#endif

static unsigned char nibble(int hex)
{
 if(hex>='0' && hex<='9') return (hex - '0');
 if(hex>='A' && hex<='F') return (hex - 'A' + 10);
 if(hex>='a' && hex<='f') return (hex - 'a' + 10);
return 0;
}

int hex2int(char *hh)
{
int nib1 = nibble(hh[0]&0xFF);
int nib2 = nibble(hh[1]&0xFF);
return  (nib1 << 4) + nib2;
}


unsigned long ulong_hex(const char *in)
{
unsigned long ul  = 0;
char *wp = (char*)in;
int i;

   for(i=0;i<4;i++)
   {
    ul <<= 8;
    ul += hex2int(wp);
    wp += 2;
   }

  return ul;
}

int event2hex(const char *in)
{
int result  = 0;
//char *wp = (char*)in;
//int i;

   result = nibble(in[0]);
   result <<= 8 ;
   result += hex2int((char*)&in[1]);

  return result;
}


