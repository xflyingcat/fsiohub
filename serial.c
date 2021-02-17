/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/
#include "includes.h"


int hex2int(char *hh);

static char lister_path[MAX_PATH+1];
typedef struct {
   char name[MAX_PATH + 1];
   int dev_id;
   HANDLE serial_handle;
} PORTS_PULL;

PORTS_PULL ports_pull[SERIAL_PORTS_MAX];

HANDLE get_handle_by_id(int id)
{
    int i;
    HANDLE tmp;
    for(i=0;i<SERIAL_PORTS_MAX;i++)
    {
       if(id == ports_pull[i].dev_id)
       {
          tmp =  ports_pull[i].serial_handle;
          ports_pull[i].serial_handle = INVALID_HANDLE_VALUE;
          return tmp;
       }
    }

    return INVALID_HANDLE_VALUE;
}

void get_name_by_id(int id, char *p)
{
    int i;
    for(i=0;i<SERIAL_PORTS_MAX;i++)
    {
       if(id == ports_pull[i].dev_id)
       {
          strcpy(p,ports_pull[i].name);
          return;
       }
    }
    *p = 0;
}

HANDLE get_handle_by_name(char *name)
{
    int i;
    HANDLE tmp;
    for(i=0;i<SERIAL_PORTS_MAX;i++)
    {
       if(0 == strcmp(ports_pull[i].name,name))
       {
          tmp =  ports_pull[i].serial_handle;
          ports_pull[i].serial_handle = INVALID_HANDLE_VALUE;
          return tmp;
       }
    }
    return INVALID_HANDLE_VALUE;
}

void close_ports_pull(void)
{
    int i;
    for(i=0;i<SERIAL_PORTS_MAX;i++)
    {
       if(ports_pull[i].serial_handle != INVALID_HANDLE_VALUE)
       {
          CloseHandle(ports_pull[i].serial_handle);
       }
    }
}


void dtr_control_on(SESSION_WORK_SPACE *sws)
{
//DCB dcb;
//     	if (GetCommState(sws->serial_handle, &dcb))
//        {//
//		    dcb.fDtrControl = TRUE;
//		    SetCommState(sws->serial_handle, &dcb);
//        }
EscapeCommFunction(sws->serial_handle,SETDTR);
}

void dtr_control_off(SESSION_WORK_SPACE *sws)
{
//DCB dcb;
//     	if (GetCommState(sws->serial_handle, &dcb))
//        {//
//		    dcb.fDtrControl = FALSE;
//		    SetCommState(sws->serial_handle, &dcb);
//        }
EscapeCommFunction(sws->serial_handle,CLRDTR);

}




int serial_open(SESSION_WORK_SPACE *sws, const char *dev_name, long baud_rate)
{

DCB dcb;
COMMTIMEOUTS commtimeouts;
char portname[32];

    sprintf(portname, "\\\\.\\%s",dev_name);

	sws->serial_handle = CreateFileA(portname,
	                          GENERIC_READ|GENERIC_WRITE,
	                          FILE_SHARE_READ|FILE_SHARE_WRITE,
	                          NULL,
	                          OPEN_EXISTING,
	                          FILE_ATTRIBUTE_NORMAL,
	                          NULL);

	if (sws->serial_handle == INVALID_HANDLE_VALUE)
	{

      return FALSE;
	}

     dcb.DCBlength = sizeof(dcb);

     commtimeouts.ReadIntervalTimeout = 50;
     commtimeouts.ReadTotalTimeoutMultiplier = 2;
     commtimeouts.ReadTotalTimeoutConstant = 100;

     commtimeouts.WriteTotalTimeoutMultiplier = 10;
     commtimeouts.WriteTotalTimeoutConstant = 10;

    if (GetCommState(sws->serial_handle, &dcb))
    {
		dcb.BaudRate = baud_rate;
		dcb.ByteSize = 8;
		dcb.StopBits = ONESTOPBIT;
		dcb.Parity = NOPARITY;
		dcb.fParity = FALSE;
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDtrControl = FALSE;
		dcb.fRtsControl = FALSE;
		dcb.fDsrSensitivity = FALSE;

		if (SetCommState(sws->serial_handle, &dcb))
        {
		    if (SetCommTimeouts(sws->serial_handle, &commtimeouts))
            {

                //Sleep(2000);
         	    //dtr_control_on(sws);
                //Sleep(2000);
         	    dtr_control_off(sws);
         	    Sleep(500);
         	    dtr_control_on(sws);
         	    return TRUE;
		    }
	    }

    }

return FALSE;
}

void serial_close(SESSION_WORK_SPACE *sws)
{
   if(sws->serial_handle != INVALID_HANDLE_VALUE &&
      sws->serial_port[0])
   {

      if(CloseHandle(sws->serial_handle))
          log_write("CloseHandle(sws->serial_handle): %s is successful",sws->serial_port);
      else
          log_write("CloseHandle(sws->serial_handle): %d",GetLastError());

   }
   sws->serial_handle = INVALID_HANDLE_VALUE;
}


int serial_write(SESSION_WORK_SPACE *sws, void *ibuf, int len)
{
DWORD n;
unsigned char *buf = ibuf;

//buf[0] = '{';
//buf[4] = '}';
//len = 5;
int sentlen = len;

   while(len)
   {
     WriteFile(sws->serial_handle,buf, len, &n, NULL);
     if(n == 0)
        return 0;
     buf += n;
     len -= n;
   }
return sentlen;
}

int serial_read(SESSION_WORK_SPACE *sws, unsigned char *buf,int len)
{
DWORD n;
    ReadFile(sws->serial_handle,
                       buf,
                       len,
                       &n,
                       NULL);
//if(n>0)
//   return n;
return n;
}


//static int listening(SESSION_WORK_SPACE *sws, SERIAL_PROTO_STRUCT *sps);
//static int on_char(SERIAL_PROTO_STRUCT *sps, int ch);

void start_serial(SESSION_WORK_SPACE *sws)
{
   int i,flush_cnt;
   //load_config("");
   log_write("USB CDC I/O Hub for FSUIPC/XPUIOC rev."PLUGIN_REVISION);
   const char *state[] = {"no ", "yes"};
   SESSION_WORK_SPACE ws;
   char detect_buf[200+1];
   int n,accessible = 0,detected = 0;
   char *p;
   ports_detect();

log_printf("--------------------+-------------+-----------+----------\n"
           "   Serial port      |  Available  |  Device   | Device\n"
           "                    |             |  detected |  ID\n"
           "--------------------+-------------+-----------+----------\n");

   for(i=0;i<SERIAL_PORTS_MAX;i++)
   {
       if(ports_pull[i].name[0])
       {
           char *p = strchr(ports_pull[i].name,'-');
           if(p)
             *p = 0;

           if(serial_open(&ws, ports_pull[i].name, 115200))
           {
              accessible = 1;
              ports_pull[i].serial_handle = ws.serial_handle;

              Sleep(1500);
              flush_cnt = 0;
              while((n = serial_read(&ws,(unsigned char*)detect_buf,200))>0)
              {
                if(++flush_cnt  == 20)
                {
                   goto print_info;
                }
              }
              n = serial_write(&ws,"{DET}",5);
              if(n == 0)
                goto print_info;
              Sleep(100);
              n = serial_read(&ws,(unsigned char*)detect_buf,200);

              if(n>0)
              {
                  detect_buf[n] = 0;
                  p = strstr(detect_buf,"!{F");
                  if(p)
                  {
                     p += 3;
                     p[2] = 0;
                     ports_pull[i].dev_id =  hex2int(p);
                     detected = 1;
                  }
                  else
                  {
                    CloseHandle(ws.serial_handle);
                    ports_pull[i].serial_handle = INVALID_HANDLE_VALUE;
                    detected = 0;
                  }

              } else
                 {
                    ports_pull[i].serial_handle = INVALID_HANDLE_VALUE;
                    CloseHandle(ws.serial_handle);
                 }
           }
print_info:
           //printf("%s %d\n",ports_pull[i].name,ports_pull[i].dev_id);
           log_printf("   %-16s |      %s    |    %s    |    %d\n",
                      ports_pull[i].name,
                      state[accessible],
                      state[detected],
                      ports_pull[i].dev_id);
                      detected = 0;
       }
       else
           break;
   }
log_printf("--------------------+-------------+-----------+----------\n");
   for(i=0;i<SERIAL_PORTS_MAX;i++)
   {
       if(sws[i].serial_port[0] == 0)
       {
           sws[i].serial_handle = get_handle_by_id(i);
           if(sws[i].serial_handle != INVALID_HANDLE_VALUE)
               get_name_by_id(i,sws[i].serial_port);
       }
       else
       {
           sws[i].serial_handle = get_handle_by_name(sws[i].serial_port);
       }
       sws[i].serial_connect_flag = 0;
       sws[i].port_id = i;
       if(sws[i].serial_port[0])
           sws[i].serial_thread_handle =
           CreateThread(NULL, 0, serial_thread, &sws[i], 0, NULL);
   }
   close_ports_pull();
}

void serial_signal_to_close(SESSION_WORK_SPACE *sws)
{
  sws->opened = 0;
}

void set_flag_opened(SESSION_WORK_SPACE *sws)
{
  sws->opened = 1;
}

int is_opened(SESSION_WORK_SPACE *sws)
{
return sws->opened;
}


void serial_stop(SESSION_WORK_SPACE *sws)
{
int i;
  log_write("serial_stop():");

   for(i=0;i<SERIAL_PORTS_MAX;i++)
   {
     if(sws[i].serial_thread_handle != INVALID_HANDLE_VALUE &&
        sws[i].serial_port[0] /* name is not empty */
        )
     {

      serial_signal_to_close(&sws[i]);
      Sleep(100);
#if 0
      if(TerminateThread(sws[i].serial_thread_handle,0))
          log_write("TerminateThread(): %s is successful",sws[i].serial_port);
      else
          log_write("TerminateThread(): %d",GetLastError());
#endif


      if(CloseHandle(sws[i].serial_thread_handle))
          log_write("CloseHandle(sws->serial_thread_handle): %s is successful",sws[i].serial_port);
      else
          log_write("CloseHandle(sws->serial_thread_handle): %d",GetLastError());
     }


     serial_close(&sws[i]);
   }
  log_write("serial_stop(): the serial handles closed");
  log_write("serial_stop(): the serials closed");
}

#define SIMPLE_FIFO_SIZE 5

typedef struct
{
  int   push_indx;
  int   pop_indx;
  ULONG64 buff[SIMPLE_FIFO_SIZE];
} SIMPLE_FIFO;

static int pipes_on = 0;
static SIMPLE_FIFO send_to_sim_fifo;
static HANDLE send_to_sim_mutex = INVALID_HANDLE_VALUE;

static int fifo_push(SIMPLE_FIFO *fifo, ULONG64 data)
{
    int indx;
    indx = fifo->push_indx + 1;
    indx = indx % SIMPLE_FIFO_SIZE;

    if (indx == fifo->pop_indx)
        return 0; // fifo is full
    fifo->buff[fifo->push_indx] = data;
    fifo->push_indx = indx;
    return 1;
}


static int fifo_pop(SIMPLE_FIFO *fifo, ULONG64 *data)
{
    int indx;
    if (fifo->push_indx == fifo->pop_indx)
        return 0; // fifo is empty
    indx = fifo->pop_indx + 1;
    indx = indx % SIMPLE_FIFO_SIZE;
    *data = fifo->buff[fifo->pop_indx];
    fifo->pop_indx = indx;
    return 1;
}



// device side

int send_to_sim(ULONG64 data)
{

 if(!pipes_on)
       return 0;

 if( send_to_sim_mutex == INVALID_HANDLE_VALUE )
     return 0;

 if(WAIT_OBJECT_0 == WaitForSingleObject(send_to_sim_mutex,INFINITE))
 {
     if(fifo_push(&send_to_sim_fifo, data))
     {
       ReleaseMutex(send_to_sim_mutex);
       return 1;
     }
  ReleaseMutex(send_to_sim_mutex);
 }
return 0;
}





// sim side

int recv_from_device(ULONG64 *data)
{
 if(!pipes_on)
       return 0;

 if( send_to_sim_mutex == INVALID_HANDLE_VALUE )
     return 0;

 if(WAIT_OBJECT_0 == WaitForSingleObject(send_to_sim_mutex,INFINITE))
 {
   if(fifo_pop(&send_to_sim_fifo, data))
   {
     ReleaseMutex(send_to_sim_mutex);
     return 1;
   }
   ReleaseMutex(send_to_sim_mutex);
 }
return 0;
}

void flush_from_device(void)
{
    ULONG64 data;
    if( send_to_sim_mutex == INVALID_HANDLE_VALUE )
        return;

    if(WAIT_OBJECT_0 == WaitForSingleObject(send_to_sim_mutex,INFINITE))
    {
        while(fifo_pop(&send_to_sim_fifo, &data));
        ReleaseMutex(send_to_sim_mutex);
    }
}


void pipes_create(void)
{

    memset(&send_to_sim_fifo,0,sizeof(SIMPLE_FIFO));

    send_to_sim_mutex    = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

}

void pipes_destroy(void)
{
    ULONG64 data;

    if( send_to_sim_mutex == INVALID_HANDLE_VALUE )
        return;


    if(WAIT_OBJECT_0 == WaitForSingleObject(send_to_sim_mutex,INFINITE))
    {
        pipes_on = 0;
        while(fifo_pop(&send_to_sim_fifo, &data));
        ReleaseMutex(send_to_sim_mutex);
    }

    CloseHandle(send_to_sim_mutex);
    send_to_sim_mutex = INVALID_HANDLE_VALUE;
    log_write("Mutex semaphore handle closed");
}

void set_pipes_off(void)
{
ULONG64 data;

 if( send_to_sim_mutex == INVALID_HANDLE_VALUE )
 {
     pipes_on = 0;
     return;
 }

 if(WAIT_OBJECT_0 == WaitForSingleObject(send_to_sim_mutex,INFINITE))
 {
      pipes_on = 0;

      while(fifo_pop(&send_to_sim_fifo, &data));
      ReleaseMutex(send_to_sim_mutex);
 }
}

void set_pipes_on(void)
{
    ULONG64 data;

    if( send_to_sim_mutex == INVALID_HANDLE_VALUE )
    {
        pipes_on = 0;
        return;
    }

    if(WAIT_OBJECT_0 == WaitForSingleObject(send_to_sim_mutex,INFINITE))
    {
        while(fifo_pop(&send_to_sim_fifo, &data));
        pipes_on = 1;
        ReleaseMutex(send_to_sim_mutex);
    }
}

#include "listComPorts.h"



int ports_detect(void)
{

    const char *pattern[] = {
          "USB\\VID",
          "FTDIBUS\\VID",
          "MOSCHIP\\COMPOUND_SERIAL_INTERFACE",
          "Electronic Team - VSBC",
          "ELTIMA Software - VSBC",
          ""
    };
    char cmdline[5*MAX_PATH];
    char exe_fname[2*MAX_PATH+1];
    char out_fname[2*MAX_PATH+1];
    char *p = getenv("TEMP");
    int i,j=0;
    printf("Getting list of serial ports\n");
    for(i = 0; i < SERIAL_PORTS_MAX; i++)
    {
        ports_pull[i].name[0] = 0;
        ports_pull[i].dev_id = -1;
        ports_pull[i].serial_handle = INVALID_HANDLE_VALUE;
    }

    if(p)
    {
        strcpy(lister_path, p);
        sprintf(cmdline,"md %s\\fsiohub",lister_path);
        system(cmdline);
        strcat(lister_path,"\\fsiohub");

        sprintf(exe_fname,"%s\\listComPorts.exe",lister_path);
        sprintf(out_fname,"%s\\port_list.txt",lister_path);

        FILE *fp = fopen(exe_fname,"wb");
        if(fp)
        {
            fwrite(listComPorts,sizeof(listComPorts),1,fp);
            fclose(fp);
            sprintf(cmdline,"%s > %s",exe_fname, out_fname);
            system(cmdline);

            //printf("\nPatterns for USB port finding:\n");
            //  printf("==============================\n");
            //for(i=0;pattern[i][0];i++)
           // {
           //     printf("%2d. %s\n",i+1, pattern[i]);
           // }

            fp = fopen(out_fname,"rb");
            if(fp)
            {
               j = 0;
               int jj = 0;
               char qq;
               printf("\nSerial ports list:\n");
                 printf("==================\n");
               while(!feof(fp))
               {
                   if(fgets(cmdline,MAX_PATH,fp))
                   {


                        //for(qq=' ',i=0;pattern[i][0];i++)
                        //{
                        //  if(strstr(cmdline, pattern[i]))
                        //  {
                        //      qq = '*';
                        //      break;
                        //  }
                        //}
                        cprintf("%c%2d. %s",' ',(jj++)+1,cmdline);

                      //if(pattern[i][0])
                      //{
                           //printf("Okay for pattern: %s\n",pattern[i]);
                           p = strstr(cmdline," - ");

                           if(p && ((p - cmdline)<MAX_PATH))
                           {
                               *p = 0;
                               if(j<SERIAL_PORTS_MAX)
                               {
                                  strcpy(ports_pull[j++].name,cmdline);
                               }

                           }

                      //}
                   }


               }
               fclose(fp);
            }

        }
    }
    return 0;
}








