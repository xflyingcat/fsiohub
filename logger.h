/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#ifndef __LOGGER_H__
#define __LOGGER_H__

int log_write(const char* str, ... );
void log_remove(void);
int log_printf(const char* str, ... );
#endif

