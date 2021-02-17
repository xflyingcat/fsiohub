
/**********************************************
 FlyingCat 2013
 Under the BEER-WARE LICENSE
 google it for more details
***********************************************/


#include "includes.h"

void main_task(void);
char logfile_path[MAX_PATH*2];
SESSION_WORK_SPACE sws[SERIAL_PORTS_MAX];

int main(int argc, char* argv[])
{
    STARTUPINFO cif;
    PROCESS_INFORMATION pi;
    char module_file[MAX_PATH+1];
    console_init("FSIOHUB rev."PLUGIN_REVISION);

    DWORD sz = GetModuleFileName(NULL, module_file, MAX_PATH);
    module_file[sz] = 0;
    if(sz>4 && module_file[sz-4] == '.')
    {
        module_file[sz-4] = 0;
        sprintf(logfile_path,"%s",module_file);
    }
    //load_config();

    if(argc > 1)
    {
        /* kind of fork() */
        if(0 == strcmp(argv[1],"AS_DAEMON"))
        {
            ZeroMemory(&cif,sizeof(STARTUPINFO));
            if(CreateProcess(NULL,module_file,
                             NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&cif,&pi))
            {
                CloseHandle( pi.hProcess );
                CloseHandle( pi.hThread );
                exit(0);
            }
        }
    }
    main_task();
    return 0;
}


void main_task(void)
{
    log_write("FSIOHUB "PLUGIN_REVISION);
    ULONG64 event;
    log_remove();
    load_config("");
    pipes_create();
    start_serial(sws);
    set_pipes_on();
    printf("Waitnig for FSUIPC\n");
    while(1)
    {
        if(fsuipc_connect())
        {
            SetConsoleTitle("FSIOHUB rev."PLUGIN_REVISION" Connected to FSUIPC");
            //printf("Connected to FSUIPC\n");
            set_fsuipc_flag();
        }
        else
        {
            //SetConsoleTitle("FSIOHUB rev."PLUGIN_REVISION" Waitnig for FSUIPC");
            //printf("Waitnig for FSUIPC\n");
            Sleep(5000);
        }

        do
        {/*  {DET}*/

            poll_offsets();
            //poll_inputs();
            WORD ch = cons_getchar();
            if(ch == 'l' || ch == 'L')
            {
               load_config("");
            }

            if((recv_from_device(&event)))
            {
                on_input_event(event);
                EVENT_STRUCT *ev = (EVENT_STRUCT*)&event;
                printf("event_type:%03d port_id:%02d event_id:%04X(",
                       ev->event_type,
                       ev->port_id,
                       ev->event_id);

                set_text_color(BRIGHT_GREEN);
                printf("%04d",ev->event_id);
                set_text_color(WHITE);


                printf(") value:%d\n",
                       ev->value
                       );
            }
            Sleep(2);

        }
        while(get_fsuipc_flag());
    }
}



