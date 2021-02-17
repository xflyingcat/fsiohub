#ifndef __CONSOLE_UTILS_H__
#define __CONSOLE_UTILS_H__

#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define RED      (FOREGROUND_RED                                      )
#define YELLOW   (FOREGROUND_RED  | FOREGROUND_GREEN                  )
#define GREEN    (                  FOREGROUND_GREEN                  )
#define BLUE     (                                     FOREGROUND_BLUE)
#define MAGENTA  (FOREGROUND_RED  |                    FOREGROUND_BLUE)
#define CYAN     (                  FOREGROUND_GREEN | FOREGROUND_BLUE)
#define WHITE    (FOREGROUND_RED  | FOREGROUND_GREEN | FOREGROUND_BLUE)

#define BRIGHT_RED      (FOREGROUND_RED                                       | FOREGROUND_INTENSITY)
#define BRIGHT_YELLOW   (FOREGROUND_RED  | FOREGROUND_GREEN                   | FOREGROUND_INTENSITY)
#define BRIGHT_GREEN    (                  FOREGROUND_GREEN                   | FOREGROUND_INTENSITY)
#define BRIGHT_BLUE     (                                     FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define BRIGHT_MAGENTA  (FOREGROUND_RED  |                    FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define BRIGHT_CYAN     (                  FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define BRIGHT_WHITE    (FOREGROUND_RED  | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)

int cprintf (const char* str, ... );
void console_init(char *title);
int cons_getstr(char *str,int size, char *default_line);
void cons_setxy(int x, int y);
void bs(void);
void set_text_color(long cl);
int cons_getchar(void);
int cp1251_to_CP866(char *str);
char *datetime_compact(void);
void error_exit(void);
void wait_pressing(void);
char *datetime_human(void);
int cons_wait_getchar(void);
void to_upper(char *str);
int cons_wait_getchar_echo(void);
void console_close(void);
int cons_getchar_ext(void);
#endif


