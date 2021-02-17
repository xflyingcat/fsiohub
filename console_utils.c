#include "windows.h"
#include "console_utils.h"
#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>

HANDLE hKey = NULL;
HANDLE hScreen = NULL;

void console_init(char *title)
{
INPUT_RECORD ir;
DWORD n;


if ((hScreen = GetStdHandle(STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	return;
if ((hKey = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	return;
 SetConsoleMode(hKey, 0);
 SetConsoleTitle(title);
 SetConsoleCP(1251);
}

void console_close(void)
{
    if(hKey)
        CloseHandle(hKey);
    if(hScreen)
        CloseHandle(hScreen);
}


int cons_getstr(char *str,int size, char *default_line)
{

int i = 0;
int ch;
int defline_len = 0;

if(default_line)
{
   if(default_line)
   {
      i = cprintf("%s",default_line);
      strcpy(str,default_line);
      defline_len = strlen(default_line);
   }
}


 while(1)
 {
       if((ch = cons_getchar()) != -1)
        {
                if(ch == 0x0A || ch == 0x0D)
                        {
                         str[i] = 0x00;
                         strcpy(str,&str[defline_len]);
                         printf("\n");
                         return strlen(str);
                        }
       else if(ch == 0x08)
                    {
                        if(i>0)
                        {
                           bs();
                           printf(" ");
                           bs();
                           i--;
                        }
                    } else
                       {
                         if(i<size)
                         {
                           cprintf("%c",ch);
                           str[i++] = ch;
                         }
                       }
            }
            ch = 0;


        }



}

void cons_setxy(int x, int y)
{
COORD coordDest;
   coordDest.Y=y;
   coordDest.X=x;
  SetConsoleCursorPosition (hScreen,coordDest);
}

void bs(void)
{
CONSOLE_SCREEN_BUFFER_INFO consinfo;

GetConsoleScreenBufferInfo(
  hScreen,
  &consinfo
);
 cons_setxy(
  consinfo.dwCursorPosition.X-1,
  consinfo.dwCursorPosition.Y);
}


void set_text_color(long cl)
{
 SetConsoleTextAttribute(hScreen, cl );
}

int cons_getchar(void)
{
int c;
char ch;
OVERLAPPED ovr;
DWORD n,last_error,res;
INPUT_RECORD ir;

		PeekConsoleInput(hKey, &ir, 1, &n);
		if (n == 0) return -1;
		ReadConsoleInput(hKey, &ir, 1, &n);
		if (ir.EventType != KEY_EVENT) return -1;
		if (ir.Event.KeyEvent.bKeyDown == FALSE) return -1;

     if (ir.Event.KeyEvent.dwControlKeyState & (ENHANCED_KEY) )
     {
     } else
       {
  c = ir.Event.KeyEvent.uChar.AsciiChar;


    if(c==0) return -1;
    return c;
       }

return -1;
}

int cons_getchar_ext(void)
{
int c;
char ch;
OVERLAPPED ovr;
DWORD n,last_error,res;
INPUT_RECORD ir;

		PeekConsoleInput(hKey, &ir, 1, &n);
		if (n == 0)
           return -1;

		ReadConsoleInput(hKey, &ir, 1, &n);
		return ir.Event.KeyEvent.wVirtualKeyCode;
}



int cons_wait_getchar_echo(void)
{
int ch;
   while(-1 == (ch = cons_getchar()))
                       Sleep(100);
  cprintf("%c",ch);
return ch;
}

int cons_wait_getchar(void)
{
int ch;
   while(-1 == (ch = cons_getchar()))
                       Sleep(100);
return ch;
}


unsigned char cp866[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,

/*       !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /    */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,

/*  0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?   */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,

/*  @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O   */
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,

/* P     Q     R     S     T     U     V     W     X     Y     Z     [     \     ]     ^     _    */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,

/* `     a     b     c     d     e     f     g     h     i     j     k     l     m     n     o    */
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,

/* p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~         */
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* €          ‚     ƒ     „     …     †     ‡     ˆ     ‰     Š     ‹     Œ          Ž         */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,

/*      ‘     ’     “     ”     •     –     —     ˜     ™     š     ›     œ          ž     Ÿ    */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,

/*       ¡     ¢     £     ¤     ¥     ¦     §     ¨     ©     ª     «     ¬     ­     ®     ¯    */
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xF0, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,

/* °     ±     ²     ³     ´     µ     ¶     ·     ¸     ¹     º     »     ¼     ½     ¾     ¿    */
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xF1, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,

/* À     Á     Â     Ã     Ä     Å     Æ     Ç     È     É     Ê     Ë     Ì     Í     Î     Ï    */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,

/* Ð     Ñ     Ò     Ó     Ô     Õ     Ö     ×     Ø     Ù     Ú     Û     Ü     Ý     Þ     ß    */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,

/* à     á     â     ã     ä     å     æ     ç     è     é     ê     ë     ì     í     î     ï    */
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,

/* ð     ñ     ò     ó     ô     õ     ö     ÷     ø     ù     ú     û     ü     ý     þ     ÿ    */
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF
};

int cp1251_to_CP866(char *str)
{
int ch;
     while(*str)
     {
      ch = *str & 255;
      ch = cp866[ch];
      *str++ = ch;
     }

}


int cprintf (const char* str, ... )
{
va_list arp;
int len = 0;
char tmp_buf[2048];

   va_start(arp, str);
    len = vsprintf(tmp_buf,str,arp);
   va_end(arp);
   cp1251_to_CP866(tmp_buf);
   fprintf(stderr, tmp_buf);

return len;
}

char *datetime_compact(void)
{
time_t     now;
struct tm  *ts;

now = time(NULL);
ts = localtime(&now);
static char date_str[100];

        sprintf(date_str, "%02d%02d%04d-%02d%02d%02d",
                                    ts->tm_mday,
                                    ts->tm_mon+1,
                                    ts->tm_year+1900,
                                    ts->tm_hour,
                                    ts->tm_min,
                                    ts->tm_sec);

return date_str;
}


char *datetime_human(void)
{
time_t     now;
struct tm  *ts;

now = time(NULL);
ts = localtime(&now);
static char date_str[100];

        sprintf(date_str, "%02d.%02d.%04d  %02d:%02d",
                                    ts->tm_mday,
                                    ts->tm_mon+1,
                                    ts->tm_year+1900,
                                    ts->tm_hour,
                                    ts->tm_min);

return date_str;
}



void to_upper(char *str)
{
  while(*str)
  {
    *str = toupper(*str);
     str++;
  }
}
