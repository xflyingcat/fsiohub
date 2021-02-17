/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#include "includes.h"

void re_register_all(void);

SESSION_WORK_SPACE sws[SERIAL_PORTS_MAX];
int output_data[MAX_KEYS];
int input_data[MAX_KEYS];
void poll_offsets(void);
void poll_inputs(void);
void menu_create(void);


unsigned long leds = 0;

void led_control(int pos, int state)
{
unsigned long mask = 1;
mask <<= pos;
    if(state)
       leds |= mask;
    else
       leds &= ~mask;
}

unsigned long get_leds_state(void)
{
  return leds;
}


void on_input_event(ULONG64 ev_container)
{
 EVENT_STRUCT *ev = (EVENT_STRUCT*)&ev_container;


 switch(ev->event_type)
 {
    case ROTARY_SWITCH:
    case ENCODER_PULSE:
       pass_to_sim(ev_container,-1);
    break;

    case RISING_EDGE:
       pass_to_sim(ev_container,1);
    break;

    case FALLING_EDGE:
       pass_to_sim(ev_container,0);
    break;
 }

}

unsigned long mask_of_size[] = {
  /*0   1     2       3         4*/
    0,0xFF,0xFFFF,0xFFFFFF,0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF


};

typedef struct {
   char   i8;
   short  i16;
   long   i32;
} INT_VARIANT;

void put_variant(void *p, int size, INT_VARIANT *vt)
{

}

void get_variant(void *p, int size, INT_VARIANT *vt)
{


}






void pass_to_sim(ULONG64 ev_container, int level)
{
    EVENT_STRUCT *ev = (EVENT_STRUCT*)&ev_container;
    long complete_ev_id = (ev->port_id << 16) + ev->event_id;
    unsigned char void_buf[8];
    long int_value,i;
    unsigned long uint_value,tmp_mask;
    float float_value;
    INT_VARIANT tmpvar;

    for(i=0;i<MAX_KEYS;i++)
    {
       if(key_config[i].mode <= 0)
       {
         break;
       }

       if(complete_ev_id == key_config[i].key_id)
       {

             switch(key_config[i].mode)
             {
                case MODE_TOGGLE_EVENT:
                case MODE_DISCRETE_EVENT:
                    read_single_fs_var(key_config[i].offset,
                                       key_config[i].size,
                                       (void*)void_buf);

                    if(key_config[i].mask != 0 && key_config[i].type == TYPE_INT)
                    {
                       uint_value = *((unsigned long*)void_buf) &
                       mask_of_size[key_config[i].size];
                       if(!key_config[i].encoder_int[level&1])
                       {
                           tmp_mask = key_config[i].mask ^ 0xFFFFFFFF;
                           uint_value &= tmp_mask;
                       }
                       else
                           uint_value |= key_config[i].mask;

                        *((unsigned long*)void_buf) =
                        uint_value & mask_of_size[key_config[i].size];
                    }
                    else
                    if(key_config[i].mask == 0 && key_config[i].type == TYPE_INT)
                    {
                       int_value = *((long*)void_buf) &
                       mask_of_size[key_config[i].size];
                       int_value = key_config[i].encoder_int[level&1];
                        ((long*)void_buf)[0] =
                        int_value & mask_of_size[key_config[i].size];
                        ((long*)void_buf)[1] = 0;
                    }
                    else
                    if(key_config[i].type == TYPE_FLOAT)
                    {
                       float_value = *((float*)void_buf);
                       float_value = key_config[i].encoder_float[level&1];
                        *((float*)void_buf) = float_value;
                    }


                    write_single_fs_var(key_config[i].offset,
                                         key_config[i].size,
                                         (void*)void_buf);

                    break;

                case MODE_DISCRETE_LOOP_EVENT:
                case MODE_DISCRETE_LIMIT_EVENT:

                    if(level != -1)
                        break;
                    read_single_fs_var(key_config[i].offset,
                                       key_config[i].size,
                                       (void*)void_buf);

                    if(key_config[i].type == TYPE_FLOAT)
                    {
                        float_value = *((float*)void_buf);
                        float_value += key_config[i].inc_float;

                        if(key_config[i].mode == MODE_DISCRETE_LOOP_EVENT)
                        {
                            if(float_value > key_config[i].max_float)
                                 float_value = key_config[i].min_float;
                            if(float_value < key_config[i].min_float)
                                 float_value = key_config[i].max_float;
                        }
                        else
                        if(key_config[i].mode == MODE_DISCRETE_LIMIT_EVENT)
                        {
                            if(float_value > key_config[i].max_float)
                                 float_value = key_config[i].max_float;
                            if(float_value < key_config[i].min_float)
                                 float_value = key_config[i].min_float;
                        }


                        *((float*)void_buf) = float_value;
                    }

                    if(key_config[i].type == TYPE_INT)
                    {
                        switch(key_config[i].size)
                        {
                            case 1:
                                int_value = *((char*)void_buf);
                            break;
                            case 2:
                                int_value = *((short*)void_buf);
                            break;
                            case 4:
                                int_value = *((long*)void_buf);
                            break;
                            default:
                                int_value = *((long*)void_buf);
                        }

                        if(key_config[i].encoding == BCD)
                        {
                            int_value = bcd2bin(int_value);
                        }

                        if(key_config[i].mode == MODE_DISCRETE_LOOP_EVENT)
                        {
                            int_value += key_config[i].inc_int;
                            if(int_value > key_config[i].max_int)
                                 int_value = key_config[i].min_int;
                            if(int_value < key_config[i].min_int)
                                 int_value = key_config[i].max_int;
                        }
                        else
                        if(key_config[i].mode == MODE_DISCRETE_LIMIT_EVENT)
                        {
                            int_value += key_config[i].inc_int;
                            if(int_value > key_config[i].max_int)
                                 int_value = key_config[i].max_int;
                            if(int_value < key_config[i].min_int)
                                 int_value = key_config[i].min_int;
                        }


                        if(key_config[i].encoding == BCD)
                        {
                           int_value = long2bcd(int_value);
                        }

                        switch(key_config[i].size)
                        {
                            case 1:
                                *((char*)void_buf) = (char)int_value;
                            break;
                            case 2:
                                *((short*)void_buf) = (short)int_value;
                            break;
                            case 4:
                                *((long*)void_buf) = (long)int_value;
                            break;
                            default:
                                *((long*)void_buf) = (long)int_value;
                        }
                    }

                    if(key_config[i].type == TYPE_UINT)
                    {
                        switch(key_config[i].size)
                        {
                            case 1:
                                int_value = *((unsigned char*)void_buf);
                            break;
                            case 2:
                                int_value = *((unsigned short*)void_buf);
                            break;
                            case 4:
                                int_value = *((unsigned long*)void_buf);
                            break;
                            default:
                                int_value = *((unsigned long*)void_buf);
                        }

                        if(key_config[i].encoding == BCD)
                        {
                            int_value = bcd2bin(int_value);
                        }

                        if(key_config[i].mode == MODE_DISCRETE_LOOP_EVENT)
                        {
                            int_value += key_config[i].inc_int;
                            if(int_value > key_config[i].max_int)
                                 int_value = key_config[i].min_int;
                            if(int_value < key_config[i].min_int)
                                 int_value = key_config[i].max_int;
                        }
                        else
                        if(key_config[i].mode == MODE_DISCRETE_LIMIT_EVENT)
                        {
                            int_value += key_config[i].inc_int;
                            if(int_value > key_config[i].max_int)
                                 int_value = key_config[i].max_int;
                            if(int_value < key_config[i].min_int)
                                 int_value = key_config[i].min_int;
                        }

                        if(key_config[i].encoding == BCD)
                        {
                           int_value = long2bcd(int_value);
                        }

                        switch(key_config[i].size)
                        {
                            case 1:
                                *((char*)void_buf) = (unsigned char)int_value;
                            break;
                            case 2:
                                *((unsigned short*)void_buf) = (unsigned short)int_value;
                            break;
                            case 4:
                                *((unsigned long*)void_buf) = (unsigned long)int_value;
                            break;
                            default:
                                *((unsigned long*)void_buf) = (unsigned long)int_value;
                        }
                    }

                    write_single_fs_var(key_config[i].offset,
                                        key_config[i].size,
                                        (void*)void_buf);

                    break;


                case MODE_NUMERIC_EVENT:

                    if(key_config[i].type == TYPE_FLOAT)
                    {
                        float_value  = (1.0 * (ev->value - key_config[i].zero_offset)) /
                                    key_config[i].factor;
                        *((float*)void_buf) = float_value;
                    }

                    if(key_config[i].type == TYPE_INT || key_config[i].type == TYPE_UINT)
                    {
                        int_value = (1.0 * (ev->value - key_config[i].zero_offset)) /
                                    key_config[i].factor;

                        int_value &= mask_of_size[key_config[i].size];


                        switch(key_config[i].size)
                        {
                            case 1:
                                *((char*)void_buf) = (char)int_value;
                            break;
                            case 2:
                                *((short*)void_buf) = (short)int_value;
                            break;
                            case 4:
                                *((long*)void_buf) = (long)int_value;
                            break;

                            case 8:
                                 if(key_config[i].offset == 0x3110)
                                 {
                                     ((long*)void_buf)[0] = key_config[i].control;
                                     ((long*)void_buf)[0] = int_value;
                                     break;
                                 }

                            default:
                                *((long*)void_buf) = (long)int_value;
                        }

                    }

                    write_single_fs_var(key_config[i].offset,
                                        key_config[i].size,
                                        (void*)void_buf);
                    break;
             }

       }


    }

}

void poll_offsets(void)
{
   int tmp_int;
   float tmp_float;
   unsigned char void_buf[8];
   long int_value,i;
   unsigned long uint_value,tmp_mask;
   float float_value;

   for(i=0;i<MAX_KEYS;i++)
   {
      if(key_config[i].mode == MODE_NOP)
        break;

      if(key_config[i].mode == MODE_NUMERIC_POLLING)
      {
           read_single_fs_var(key_config[i].offset,
                   key_config[i].size,
                   (void*)void_buf);

           if(key_config[i].type == TYPE_INT)
           {
                if(key_config[i].factor == 0)
                    key_config[i].factor = 1;

                        switch(key_config[i].size)
                        {
                            case 1:
                                int_value = *((char*)void_buf);
                            break;
                            case 2:
                                int_value = *((short*)void_buf);
                            break;
                            case 4:
                                int_value = *((long*)void_buf);
                            break;
                            default:
                                int_value = *((long*)void_buf);
                        }


                output_data[i] = (1.0* int_value) *
                         key_config[i].factor +
                         key_config[i].zero_offset;
           }

           if(key_config[i].type == TYPE_UINT)
           {
                if(key_config[i].factor == 0)
                    key_config[i].factor = 1;

                        switch(key_config[i].size)
                        {
                            case 1:
                                int_value = *((unsigned char*)void_buf);
                            break;
                            case 2:
                                int_value = *((unsigned short*)void_buf);
                            break;
                            case 4:
                                int_value = *((unsigned long*)void_buf);
                            break;
                            default:
                                int_value = *((unsigned long*)void_buf);
                        }


                output_data[i] = (1.0* int_value) *
                         key_config[i].factor +
                         key_config[i].zero_offset;
           }

           if(key_config[i].type == TYPE_FLOAT)
           {
                if(key_config[i].factor == 0)
                    key_config[i].factor = 1;
                float_value = *((float*)void_buf);
                output_data[i] = float_value *
                         key_config[i].factor +
                         key_config[i].zero_offset;
           }

           if(key_config[i].type == TYPE_STRING)
           {
                if(key_config[i].factor == 0)
                    key_config[i].factor = 1;

                void_buf[key_config[i].size -1] = 0;
                float_value = atof(void_buf);
                output_data[i] = float_value *
                         key_config[i].factor +
                         key_config[i].zero_offset;
           }


      }

      if(key_config[i].mode == MODE_DISCRETE_POLLING)
      {
           read_single_fs_var(key_config[i].offset,
                  key_config[i].size,
                  (void*)void_buf);
           if(key_config[i].type == TYPE_INT)
           {
                int_value = *((long*)void_buf) &
                mask_of_size[key_config[i].size];
                if(key_config[i].mask)
                {
                  output_data[i] =
                      (((unsigned long)int_value) & key_config[i].mask) && 1;
                }
                else
                {
                  output_data[i] = int_value && 1;
                }
           }

           if(key_config[i].type == TYPE_FLOAT)
           {
                float_value = *((float*)void_buf);
                output_data[i] = (tmp_float > 0.5) && 1;
           }
      }

   }

}


void poll_inputs(void)
{

}



