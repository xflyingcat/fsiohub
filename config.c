/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#include "includes.h"

CONFIG config;
static int cfg_item = -1;
static int ind_item = -1;
static int input_item = -1;
static int errors_cnt = 0;
static int port = 0;

typedef struct
{
    const char *group;
    const char *key;
    char const *var;
    int size;
} INIT_STRUCT;

static const DICT modes[] =
{
    {"NOP",                  MODE_NOP},
    {"DISCRETE_EVENT",       MODE_DISCRETE_EVENT},
    {"DISCRETE_LOOP_EVENT",  MODE_DISCRETE_LOOP_EVENT},
    {"DISCRETE_LIMIT_EVENT", MODE_DISCRETE_LIMIT_EVENT},
    {"TOGGLE_EVENT",         MODE_TOGGLE_EVENT},
    {"NUMERIC_EVENT",        MODE_NUMERIC_EVENT},
    {"DISCRETE_POLLING",     MODE_DISCRETE_POLLING},
    {"NUMERIC_POLLING",      MODE_NUMERIC_POLLING},
    {"",-1}
};

static const char *printable_modes[] =
{
    "NOP",
    "DSEV",
    "LOOP",
    "TGEV",
    "NUME",
    "DPOLL",
    "NPOLL"
};
static const char *printable_types[] =
{
    "INT",
    "UINT",
    "FLT",
    "STR"
};

static const char *printable_encodings[] =
{
    "DEC",
    "BCD",
    "HEX"
};


static const DICT types[] =
{
    {"INT",     TYPE_INT},
    {"UINT",    TYPE_UINT},
    {"FLOAT",   TYPE_FLOAT},
    {"STRING",  TYPE_STRING},
    {"",-1}
};

static const DICT numeric_form_names[] =
{
    {"DEC", DEC},
    {"BCD", BCD},
    {"HEX", HEX},
    {"",-1}
};

static const DICT sections_names[] =
{
    {"KEY", SECTION_INPUT},
    {"IND", SECTION_OUTPUT},
    {"AXIS",SECTION_INPUT_AXIS},
    {"",-1}
};



KEY_CONFIG key_config[MAX_KEYS];


int key_conversion(int ev, int port)
{
    //int ev_type = (ev << 4) & 0xF000;
    //ev &= 0xFF;
    //ev |= (ev_type | (port << 8));

    return (port << 16) | (ev & 0xFFFF);
}

char *upper_case(char *str)
{
    int i;
    for(i=0; i<strlen(str); i++)
    {
        str[i] = toupper(str[i]);

    }

    return str;
}


int find_key(const DICT *dct, char *key)
{
    int i;
    for(i=0; i<strlen(key); i++)
    {
        key[i] = toupper(key[i]);
    }

    for(i=0; dct[i].key[0]; i++)
    {
        if(0 == strcmp(dct[i].key,key))
            return dct[i].val;
    }

    return -1;
}

int find_key_ext(const DICT *dct, char *key, int key_len)
{
    int i,j,flag;
    for(i=0; i<strlen(key); i++)
    {
        key[i] = toupper(key[i]);
    }


    for(i=0; dct[i].key[0]; i++)
    {
        if(strlen(dct[i].key) > key_len)
            continue;

        for(flag=0,j=0; j<key_len; j++)
        {
            if(dct[i].key[j] == key[j])
                flag++;
        }

        if(flag == key_len)
            return dct[i].val;
    }

    return -1;
}


#define ADD_ITEM(x,y,z)  {x,y,config.z,sizeof(config.z)}

static const INIT_STRUCT init_struct[] =
{

    ADD_ITEM("MODE",       "LOG",   mode_log),
    ADD_ITEM("DEBUG",      "KEYINFO",  debug_keyinfo),
    ADD_ITEM("OPERATION",  "RELOAD_KEY",  reload_key),
    ADD_ITEM("OPERATION",   "INDICATION",  indication),
    {NULL,NULL,NULL,-1}
};

static int config_cb(int lineno, void* user, const char* section, const char* name,
                     const char* value)
{
    int i;
    int key_id,mode,type;
    char pattern[100];
    int encoding = 0;
    upper_case(section);
    upper_case(name);
    upper_case(value);
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    for(i=0; init_struct[i].size!=-1; i++)
    {
        if(MATCH(init_struct[i].group, init_struct[i].key))
        {
            strncpy((char*)init_struct[i].var,value,init_struct[i].size);
            return 1;
        }
    }

    if(MATCH("DEVICE","PORT"))
    {
        port = atoi(value);
        set_port_using_flag(port);
        return 1;
    }

    if(MATCH("DEVICE","NUMERIC_OUTS_MAX"))
    {
        sws[port].numeric_outs_max = atoi(value);
         return 1;
    }

    if(MATCH("DEVICE","DISCRETE_OUTS_MAX"))
    {
        sws[port].discrete_outs_max = atoi(value);
         return 1;
    }


    for(i=0; i<SERIAL_PORTS_MAX; i++)
    {
        sprintf(pattern,"PORT_%d",i);
        if(strcmp(section, "SERIAL") == 0 && strcmp(name, pattern) == 0)
        {
            strcpy(sws[i].serial_port,value);
            return 1;

        }

    }

    if(  find_key_ext(sections_names,section,3) == SECTION_INPUT ||
            find_key_ext(sections_names,section,3) >= SECTION_OUTPUT
      )
    {

        if(0 == strcmp(name, "MODE"))
        {

            if(find_key_ext(sections_names,section,3) == SECTION_INPUT)
            {
                sscanf(&section[3],"%d",&key_id);
                key_id = key_conversion(key_id, port);
            }
            else if(find_key_ext(sections_names,section,4) == SECTION_INPUT_AXIS)
            {
                input_item++;
                key_id = input_item;
            }
            else if(find_key_ext(sections_names,section,3) == SECTION_OUTPUT)
            {
                ind_item++;
                key_id = ind_item;
            }

            cfg_item++;
            key_config[cfg_item].key_id = key_id;
            mode = find_key(modes,(char*)value);
            if(mode == -1)
            {
                key_config[cfg_item].port = -1;
                key_config[cfg_item].mode = -1;
                set_text_color(BRIGHT_RED);
                log_write("Line: %d: Error! Unknown mode: %s",lineno,value);
                errors_cnt++;
                set_text_color(WHITE);
            }
            else
            {
                key_config[cfg_item].port = port;
                key_config[cfg_item].mode = mode;
            }

        }
        else if(0 == strcmp(name, "OFFSET"))
        {
            sscanf(value,"%X",&(key_config[cfg_item].offset));
        }
        else if(0 == strcmp(name, "SIZE"))
        {
            key_config[cfg_item].size = atoi(value);
        }
        else if(0 == strcmp(name, "TYPE"))
        {
            type = find_key(types,(char*)value);
            if(type == -1)
            {
                set_text_color(BRIGHT_RED);
                log_write("Line %d: Error! Unknown type: %s",lineno,type);
                errors_cnt++;
                set_text_color(WHITE);
            }
            else
            {
                key_config[cfg_item].type = type;
            }

        }
        else if((0 == strcmp(name, "DEV")) || (0 == strcmp(name, "AXIS")))
        {
            key_config[cfg_item].devadr = atoi(value);
        }
        else if(0 == strcmp(name, "MASK"))
        {
            key_config[cfg_item].mask = atoi(value);
        }
        else if(0 == strcmp(name, "POS"))
        {
            key_config[cfg_item].pos = atoi(value);
        }
        else if(0 == strcmp(name, "PORT"))
        {
            key_config[cfg_item].port = atoi(value);
        }
        else if(0 == strcmp(name, "LEN"))
        {
            key_config[cfg_item].len = atoi(value);
        }
        else if(0 == strcmp(name, "FACTOR"))
        {
            key_config[cfg_item].factor = atof(value);
        }
        else if(0 == strcmp(name, "ZERO_OFFSET"))
        {
            key_config[cfg_item].zero_offset = atof(value);
        }
        else if(0 == strcmp(name, "OPT"))
        {
            key_config[cfg_item].opt = atoi(value);
        }
        else if(0 == strcmp(name, "CONTROL"))
        {
            key_config[cfg_item].control = atoi(value);
        }
        else if(0 == strcmp(name, "ENCODING"))
        {
            encoding = find_key(numeric_form_names,(char*)value);
            if(encoding == -1)
            {
                set_text_color(BRIGHT_RED);
                log_write("Line %d: Error! Unknown encoding: %s",lineno, encoding);
                errors_cnt++;
                set_text_color(WHITE);
            }
            else
            {
                key_config[cfg_item].encoding = encoding;
            }


            key_config[cfg_item].opt = atoi(value);
        }
        else if(0 == strcmp(name, "MIN"))
        {
            if(key_config[cfg_item].type == TYPE_INT || key_config[cfg_item].type == TYPE_UINT)
            {
                key_config[cfg_item].min_int = atoi(value);
            }
            else if(key_config[cfg_item].type == TYPE_FLOAT)
            {
                key_config[cfg_item].min_float = atof(value);
            }
        }
        else if(0 == strcmp(name, "MAX"))
        {
            if(key_config[cfg_item].type == TYPE_INT || key_config[cfg_item].type == TYPE_UINT)
            {
                key_config[cfg_item].max_int = atoi(value);
            }
            else if(key_config[cfg_item].type == TYPE_FLOAT)
            {
                key_config[cfg_item].max_float = atof(value);
            }
        }
        else if(0 == strcmp(name, "INCREMENT"))
        {
            if(key_config[cfg_item].type == TYPE_INT || key_config[cfg_item].type == TYPE_UINT)
            {
                key_config[cfg_item].inc_int = atoi(value);
            }
            else if(key_config[cfg_item].type == TYPE_FLOAT)
            {
                key_config[cfg_item].inc_float = atof(value);
            }
        }
        else if(0 == strcmp(name, "DIGITS_NUMBER"))
        {
            key_config[cfg_item].max_int = atoi(value);
        }
        else if(0 == strcmp(name, "0"))
        {
            if(key_config[cfg_item].type == TYPE_INT || key_config[cfg_item].type == TYPE_UINT)
            {
                key_config[cfg_item].encoder_int[0] = atoi(value);
            }
            else if(key_config[cfg_item].type == TYPE_FLOAT)
            {
                key_config[cfg_item].encoder_float[0] = atof(value);
            }
        }
        else if(0 == strcmp(name, "1"))
        {
            if(key_config[cfg_item].type == TYPE_INT || key_config[cfg_item].type == TYPE_UINT)
            {
                key_config[cfg_item].encoder_int[1] = atoi(value);
            }
            else if(key_config[cfg_item].type == TYPE_FLOAT)
            {
                key_config[cfg_item].encoder_float[1] = atof(value);
            }
        }
        else
        {
           set_text_color(BRIGHT_RED);
           log_write("Line %d: Error! Unknown keyword: %s",lineno, name);
           errors_cnt++;
           set_text_color(WHITE);
        }

    }
    else
    {
       set_text_color(BRIGHT_RED);
       log_write("Line %d: Error! Unknown section name: %s",lineno, section);
       errors_cnt++;
       set_text_color(WHITE);
       return 0;
    }
    return 1;
}


int load_config(char *path)
{
    static int flag = 0;
    memset(key_config,0,sizeof(key_config));
    cfg_item = -1;
    ind_item = -1;
    int i;
    input_item = -1;
    port = 0;
    log_write("Config file loading");

    clear_ports_using_flags();

    char ini_path[MAX_PATH];
    strcpy(ini_path,path);

    strcat(ini_path, MCP_INI);

    if(ini_parse(ini_path, config_cb, &config) < 0)
    {
        log_write("could not load config file %s",ini_path);
        return 0;
    }
    if(!flag)
    {
       #if 0
        log_printf("*****************************\n");
        log_printf("*     I/O configuration\n");
        log_printf("*     Items: %d\n",cfg_item+1);
        log_printf("*****************************\n");
        for(i=0; !flag&&(i<=cfg_item); i++)
        {
          print_config_line(&key_config[i]);
        }
       #endif
    }

    flag = 1;
    if(config.reload_key[0])
        sscanf(config.reload_key,"%d",&config.reload_code);
    else
        config.reload_code = -1;


  log_printf("\nTotal config items:   %d\n",cfg_item+1);
    log_printf("Total input items:    %d\n",input_item+1);
    log_printf("Total output items:   %d\n",ind_item+1);
    log_printf("Config errors:        %d\n\n",errors_cnt);
    if(errors_cnt)
    {
        printf("Config loaded with errors, press any key to exit");
        //cons_wait_getchar();
        //exit(0);
    }
    else
    {
        set_text_color(BRIGHT_GREEN);
        printf("Config loaded with no errors\n");
        set_text_color(WHITE);
    }

    return 1;
}




void print_config_line(KEY_CONFIG *cfg_item)
{
    char buf[1000],*p;
    p = buf;
    p += sprintf(p,"\nid:%04d ",cfg_item->key_id);
    p += sprintf(p,"offset:%04X ",cfg_item->offset);
    p += sprintf(p,"size:%d ",cfg_item->size);
    p += sprintf(p,"type:%s ",printable_types[cfg_item->type]);
    p += sprintf(p,"mode:%s ",printable_modes[cfg_item->mode]);
    p += sprintf(p,"minf:%7.3f ",cfg_item->min_float);
    p += sprintf(p,"maxf:%7.3f ",cfg_item->max_float);
    p += sprintf(p,"incf:%7.3f ",cfg_item->inc_float);
    p += sprintf(p,"mini:%d ",cfg_item->min_int);
    p += sprintf(p,"maxi:%d ",cfg_item->max_int);
    p += sprintf(p,"inci:%d ",cfg_item->inc_int);
    p += sprintf(p,"statf:%7.3f ",cfg_item->state_float);
    p += sprintf(p,"port:%d ", cfg_item->port);
    p += sprintf(p,"enci:{%d,%d} ",cfg_item->encoder_int[0],cfg_item->encoder_int[1]);
    p += sprintf(p,"encf:{%7.3f,%7.3f} ",cfg_item->encoder_float[0],cfg_item->encoder_float[1]);
    p += sprintf(p,"dev:%d ", cfg_item->devadr);
    p += sprintf(p,"pos:%d ",cfg_item->pos);
    p += sprintf(p,"len:%d ", cfg_item->len);
    p += sprintf(p,"factor:%7.3f ", cfg_item->factor);
    p += sprintf(p,"zoffset:%7.3f ",cfg_item->zero_offset);
    p += sprintf(p,"opt:%d ",cfg_item->opt);
    p += sprintf(p,"encoding:%s ",printable_encodings[cfg_item->encoding]);
    p += sprintf(p,"mask:%08X", cfg_item->mask);
    p += sprintf(p,"control:%d", cfg_item->control);
    log_printf("%s\n",buf);

}

