/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#include "includes.h"

int log_write(const char* str, ... );
void set_log_file_path(char *path);


void log_remove(void)
{
  system("del /F /Q "MCP_LOG);
}


int log_write(const char* str, ... )
{

struct tm *tm_tmp;
time_t lt;
va_list arp;
FILE* fp;
char datetime[40];
  lt = time(NULL);
  tm_tmp = localtime(&lt);
  strftime(datetime, 31,"%d.%m.%Y-%H:%M:%S ",tm_tmp);

//if(0==strcmp(config.mode_log,"on") || 0==strcmp(config.mode_log,"ON"))
{
  if((fp = fopen(MCP_LOG,"a")))
  {
   fprintf(fp,datetime);
   va_start(arp, str);
     vfprintf(fp,str,arp);
     vprintf(str,arp);
   va_end(arp);
   fprintf(fp,"\n");
   printf("\n");
   fclose(fp);
  }


}
return 0;
}


int log_printf(const char* str, ... )
{

va_list arp;
FILE* fp;

if(0==strcmp(config.mode_log,"on") || 0==strcmp(config.mode_log,"ON"))
{
  if((fp = fopen(MCP_LOG,"a")))
  {
   va_start(arp, str);
     vfprintf(fp,str,arp);
     vprintf(str,arp);
   va_end(arp);
   fclose(fp);
  }
}
return 0;
}


