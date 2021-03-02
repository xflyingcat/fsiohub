/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#include "includes.h"


char *get_fmt(char *fmt_buf, int len)
{

  fmt_buf[0] = '\%';
  fmt_buf[1] = '0'+len;
  fmt_buf[2] = 'd';
  fmt_buf[3] = 0x00;

return fmt_buf;
}

char *get_fmt_hex(char *fmt_buf, int len)
{

  fmt_buf[0] = '\%';
  fmt_buf[1] = '0';
  fmt_buf[2] = '0'+len;
  fmt_buf[3] = 'X';
  fmt_buf[4] = 0x00;

return fmt_buf;
}


char *get_fmt0(char *fmt_buf, int len)
{
  fmt_buf[0] = '\%';
  fmt_buf[1] = '0';
  fmt_buf[2] = '0'+len;
  fmt_buf[3] = 'd';
  fmt_buf[4] = 0x00;

return fmt_buf;
}






void display_update(SESSION_WORK_SPACE *sws)
{
#if 1
    char buf[1+3+8*48+1];
    int i,key_code;
    char tmp_buf[16];
    char fmt_buf[16];
    int tmp_int;

   int numeric_type_len = 128;
   if(sws->numeric_outs_max)
       numeric_type_len = sws->numeric_outs_max;
   int devs = numeric_type_len / 8;

   memset(buf,' ',sizeof(buf));
   for(i=0;i<MAX_KEYS;i++)
   {
    if(key_config[i].mode == MODE_NOP)
      break;

    if(key_config[i].port == sws->port_id)
    {

       if(key_config[i].mode == MODE_NUMERIC_POLLING && key_config[i].len)
       {
          key_code = i;//key_config[i].key_id;

       if(key_config[i].encoding == HEX || key_config[i].encoding == BCD)
       {
         sprintf(tmp_buf,get_fmt_hex(fmt_buf,key_config[i].len),output_data[key_code]);
       }
       else
       {
         if(key_config[i].opt & 1)
           sprintf(tmp_buf,get_fmt0(fmt_buf,key_config[i].len),output_data[key_code]);
         else
           sprintf(tmp_buf,get_fmt(fmt_buf,key_config[i].len),output_data[key_code]);
       }
        //((devs-1)-key_config[i].devadr)
        memcpy(&buf[
                      4+8*key_config[i].devadr
                        +  key_config[i].pos],
              tmp_buf,
              key_config[i].len);
       }
    }
   }
      buf[0] = '{';
      buf[1] = '7';
      buf[2] = 'S';
      buf[3] = 'G';
      buf[132] = '}';
      buf[numeric_type_len + 4] = '}';
      serial_write(sws,(unsigned char*)buf,numeric_type_len + 5);
#endif
}

void lamps_update(SESSION_WORK_SPACE *sws)
{
#if 1
   char buf[1+3+8*128+1];
   int i,key_code,n;

   memset(buf,' ',sizeof(buf));
   int discrete_type_len = 128;
   if(sws->discrete_outs_max)
      discrete_type_len = sws->discrete_outs_max;

   for(i=0;i<MAX_KEYS;i++)
   {
     if(key_config[i].mode == MODE_NOP)
        break;
     if(key_config[i].port == sws->port_id)
     {
       if(key_config[i].mode == MODE_DISCRETE_POLLING && key_config[i].len == 0)
       {
         key_code = i;
         buf[1+3+ key_config[i].pos] = '0' + (output_data[key_code] & 1);
       }

     }
   }

      buf[0] = '{';
      buf[1] = 'L';
      buf[2] = 'E';
      buf[3] = 'D';
      buf[discrete_type_len+4] = '}';
      n = serial_write(sws,(unsigned char*)buf,discrete_type_len+5);

      if(n <= 0)
      {
         sws->error_flag = 1;
         log_write("Error of writing to serial port %s",sws->serial_port);
      }
#endif
}

void lamps_and_displays_update(SESSION_WORK_SPACE *sws)
{
   if(!get_fsuipc_flag())
     return;

    if(++(sws->disp_cnt) & 1)
      display_update(sws);
    else
      lamps_update(sws);
}


static int on_char(SESSION_WORK_SPACE *sws, SERIAL_PROTO_STRUCT *sps, int ch)
{
           switch(ch)
           {
              case '!': /* ACKed last transmittion*/
                 sws->to = 0;
                 lamps_and_displays_update(sws);
              break;

              case STX:
                sps->cnt = 0;
              break;

              case ETX:
                 sps->len = sps->cnt;
                 return 1;
              break;

              default:
              {
                if(sps->cnt < SERIAL_PROTO_BUF_SIZE)
                {
                  sps->buf[sps->cnt++] = ch;
                }
                else
                  sps->cnt = 0;
              }
            }
return 0;
}

static int listening(SESSION_WORK_SPACE *sws, SERIAL_PROTO_STRUCT *sps)
{
int len;
unsigned char buf[SERIAL_PROTO_BUF_SIZE];
   if((len = serial_read(sws,buf,1)>0))
   {
       if(on_char(sws, sps, buf[0]))
       {
         sps->buf[sps->len] = 0x00;
         return 1;
       }
   }
return 0;
}

int convert_to_int(char * buf)
{
    if(!buf)
        return 0;
    char tmp[6];
    memcpy(tmp, buf, 5);
    tmp[5] = 0;
    return atoi(tmp);
}



DWORD WINAPI serial_thread(LPVOID parms)
{
   SESSION_WORK_SPACE	*sws = (SESSION_WORK_SPACE*)parms;
   SERIAL_PROTO_STRUCT sps;
   memset(&sps,0,sizeof(SERIAL_PROTO_STRUCT));
   unsigned long event,ev_type, extra_value = 0;
   sws->live_counter = 0;
   ULONG64 ev_container;
   EVENT_STRUCT *ev = (EVENT_STRUCT*)&ev_container;

   sws->to = 15;
   int cnv_cnt;
   char *p;
  if(sws->serial_port[0] == 0 && sws->serial_handle == INVALID_HANDLE_VALUE)
  {
    return 0;
  }

   if(sws->serial_handle == INVALID_HANDLE_VALUE)
   {
    if(serial_open(sws,sws->serial_port,BAUD_RATE))
    {

    }
    else
    {
      sws->serial_handle = INVALID_HANDLE_VALUE;
      sws->serial_thread_handle = INVALID_HANDLE_VALUE;
      return 0;
    }
   }

      sws->serial_connect_flag = 1;
      set_flag_opened(sws);

      log_write("Thread for device:%d serial port: %s started",
                sws->port_id,
                sws->serial_port);
      while(is_opened(sws))
      {
         sws->live_counter++;

         if(listening(sws,&sps))
         {
           sps.buf[sps.len] = 0x00;
           if(sps.buf[0] == 'A' && sps.buf[1] == 'X'/* && 82 == strlen(sps.buf)*/)
           {
             for(cnv_cnt = 0, p = &sps.buf[2]; cnv_cnt < 16; cnv_cnt++, p+=5)
             {
              input_data[sws->port_id*16+cnv_cnt] = convert_to_int(p);
             }
               continue;
           }
           /*
             simple event     TXX          801
                                           00008d01
                              01234567
             extneded event   TXXxxxx     A013039 Event A control 01 value 12345
                                           3039Ad01 Value 12345 in hex, Event A, device d, control 01


           */
           switch(sps.len)
           {
              case 3:  /* 801 */
                 event = event2hex(sps.buf);
                 ev->event_type =  event >> 8;
                 ev->event_id = event & 0xFF;
                 ev->port_id = sws->port_id;
                 ev->value = 0;
              break;

              case 4:   /* 8001*/
                 event = event2hex(sps.buf);
                 ev->event_type =  event >> 12;
                 ev->event_id = event & 0xFFF;
                 ev->port_id = sws->port_id;
                 ev->value = 0;
              break;

              case 7: /* A011234*/
                 sps.buf[7] = '0';
                 sps.buf[7] = 0x00;
                 event = ulong_hex(sps.buf);
                 event >>= 4;
                 ev->event_type =  event >> 24;
                 ev->event_id = (event >> 16) & 0xFF;
                 ev->port_id = sws->port_id;
                 ev->value = event & 0xFFFF;
              break;

              case 8:  /* A0011234*/
                 event = ulong_hex(sps.buf);
                 ev->event_type =  event >> 28;
                 ev->event_id = (event >> 16) & 0xFFF;
                 ev->port_id = sws->port_id;
                 ev->value = event & 0xFFFF;
              break;

           }
/*
           event = event2hex(sps.buf);
           ev_type = (event << 4) & 0xF000;

           if(ev_type == 0xA000)
           {
              sps.buf[8] = 0;
              extra_value = atol(&(sps.buf[3]));
              extra_value <<= 16;
           }
           event &= 0xFF;
           event |= (ev_type | (sws->port_id << 8));
           event |= extra_value;
*/
           send_to_sim(ev_container);
         }
         else
         {
            if(++(sws->to) == 20 && sws->port_id != 7)
            {
              sws->to = 0;
              lamps_and_displays_update(sws);
            }

         }

         if(sws->error_flag)
         {
           CloseHandle(sws->serial_handle);
           while(!serial_open(sws,sws->serial_port,BAUD_RATE))
           {
              log_write("Trying of port %s opening",sws->serial_port);
              Sleep(5000);
           }
           sws->error_flag = 0;
           log_write("Connected to serial port %s",sws->serial_port);
         }
      }
      log_write("Thread for serial port %s stopped",sws->serial_port);
return 0;
}



