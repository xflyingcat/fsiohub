#include "includes.h"
#include "FSUIPC_User.h"

void read_all_vars(void);
void fs_poll_thread(unsigned char *recvbuf,int len);

char *pszErrors[] =
	{	"Okay",
		"Attempt to Open when already Open",
		"Cannot link to FSUIPC or WideClient",
		"Failed to Register common message with Windows",
		"Failed to create Atom for mapping filename",
		"Failed to create a file mapping object",
		"Failed to open a view to the file map",
		"Incorrect version of FSUIPC, or not FSUIPC",
		"Sim is not version requested",
		"Call cannot execute, link not Open",
		"Call cannot execute: no requests accumulated",
		"IPC timed out all retries",
		"IPC sendmessage failed all retries",
		"IPC request contains bad data",
		"Maybe running on WideClient, but FS not running on Server, or wrong FSUIPC",
		"Read or Write request cannot be added, memory for Process is full",
	};


int fsuipc_connect(void)
{	DWORD dwResult;

	if (FSUIPC_Open(SIM_ANY, &dwResult))
	{	//char chMsg[128], chTimeMsg[64];
		char chTime[3];
		BOOL fTimeOk = TRUE;
		static char *pFS[] = { "FS98", "FS2000", "CFS2", "CFS1", "Fly!", "FS2002", "FS2004","FSX","P3D","P3D","FSX","FSX" };  // Change made 060603

		// Okay, we're linked, and already the FSUIPC_Open has had an initial
		// exchange with FSUIPC to get its version number and to differentiate
		// between FS's.

		// Now to auto-Register with FSUIPC, to save the user of an Unregistered FSUIPC
		// having to Register UIPCHello for us:
		static char chOurKey[] = "IKB3BI67TCHE"; // As obtained from Pete Dowson

		if (FSUIPC_Write(0x8001, 12, chOurKey, &dwResult))
			FSUIPC_Process(&dwResult); // Process the request(s)
long tmplong=1;

		if (FSUIPC_Write(0x07BC, 4, &tmplong, &dwResult))
			FSUIPC_Process(&dwResult); // Process the request(s)


		// I've not checked the reslut of the above -- if it didn't register us,
		// and FSUIPC isn't fully user-Registered, the next request will not
		// return the FS lock time

		// As an example of retrieving data, well also get the FS clock time too:
		if (!FSUIPC_Read(0x238, 3, chTime, &dwResult) ||
				// If we wanted other reads/writes at the same time, we could put them here
				!FSUIPC_Process(&dwResult)) // Process the request(s)
			fTimeOk = FALSE;

		// Now display all the knowledge we've accrued:
		if (fTimeOk)
			log_write("Request for time ok: FS clock = %02d:%02d:%02d", chTime[0], chTime[1], chTime[2]);

		else
			log_write("Request for time failed: %s", pszErrors[dwResult]);




		log_write("Sim is %s,   FSUIPC Version = %c.%c%c%c%c",
			(FSUIPC_FS_Version && (FSUIPC_FS_Version <= 10)) ? pFS[FSUIPC_FS_Version - 1] : "Unknown FS", // Change made 060603
			'0' + (0x0f & (FSUIPC_Version >> 28)),
			'0' + (0x0f & (FSUIPC_Version >> 24)),
			'0' + (0x0f & (FSUIPC_Version >> 20)),
			'0' + (0x0f & (FSUIPC_Version >> 16)),
			(FSUIPC_Version & 0xffff) ? 'a' + (FSUIPC_Version & 0xff) - 1 : ' ');
		log_write("Link established to FSUIPC") ;
		return 1;
	}

	else
    {
        SetConsoleTitle("FSIOHUB rev."PLUGIN_REVISION"    Failed to open link to FSUIPC");
    }


	FSUIPC_Close(); // Closing when it wasn't open is okay, so this is safe here
   return 0 ;
}

int write_single_fs_var(int offset,int size,void *val)
{
DWORD dwResult;
      if(
         FSUIPC_Write(offset,
                     size,
                     val,
                     &dwResult)
        )
       {

	     FSUIPC_Process(&dwResult);
         if(dwResult)
         {
           printf("%s\n",pszErrors[dwResult]);
           reset_fsuipc_flag();
           return 0;
         }
       }
return 1;
}

int read_single_fs_var(int offset,int size,void *val)
{
DWORD dwResult;
      if(
         FSUIPC_Read(offset,
                     size,
                     val,
                     &dwResult)
        )
       {

	     FSUIPC_Process(&dwResult);
         if(dwResult)
         {
           printf("%s\n",pszErrors[dwResult]);
           reset_fsuipc_flag();
           return 0;
         }
       }
return 1;
}

unsigned long bcd2bin(unsigned long bcd)
{
return
  10000000 *  ((bcd & 0xF0000000) >> 28) +
   1000000 *  ((bcd & 0xF000000) >> 24) +
    100000 *  ((bcd & 0xF00000) >> 20) +
     10000 *  ((bcd & 0xF0000) >> 16) +
      1000 *  ((bcd & 0xF000) >> 12) +
       100 *  ((bcd & 0xF00) >> 8) +
        10 *  ((bcd & 0xF0) >> 4) +
         1 *   (bcd & 0xF);
}

unsigned short bin2bcd(unsigned short bin)
{
unsigned short result;
result = 256 *  (bin/100);
result +=  16 * ((bin%100)/10);
result +=   1 * ((bin%100)%10);
return result;
}

unsigned long long2bcd (unsigned int val)
{
  unsigned long bcdresult = 0; char i;


  for (i = 0; val; i++)
  {
    ((char*)&bcdresult)[i / 2] |= i & 1 ? (val % 10) << 4 : (val % 10) & 0xf;
    val /= 10;
  }
  return bcdresult;
}





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

static int fsuipc_active = 0;

void reset_fsuipc_flag(void)
{
  fsuipc_active = 0;
}

void set_fsuipc_flag(void)
{
  fsuipc_active = 1;
}

int get_fsuipc_flag(void)
{
  return  fsuipc_active;
}


int event2hex(const char *in)
{
int result  = 0;
  sscanf(in,"%X",&result);
  return result;
}
