#include <iostream> 
#include <fstream> 
#include <string> 
#include "fwlib32.h" 
#include <cstdlib> 
#include <cstdio> 
#include <cstring> 
//#define USHORT unsigned short #define MAXLEN 1280 
using namespace std;

//short DownloadFile (USHORT h, const char *filepath);
//short UploadFile (USHORT h, short progID, const char *filePath);
void HexDump (char *buf, int len, int addr);

int
main (int argc, char **argv)
{
  unsigned short h = 1;
  short ret;
  string ip = "192.168.0.190";

  if (argc > 1)
    {
      ip = argv[1];
    }

  if (EW_OK != cnc_startupprocess (0, "/var/log/foces32.log"))
    {
      printf ("statup proceess fialed");
      return -1;
    }
  cout << "ip: " << ip << endl;
  if (EW_OK != cnc_allclibhndl3 (ip.c_str (), 8193, 5, &h))	/// Allocate the library handle(for Ethernet connection)
    {
      printf ("Connect Failed!\n");
      system ("Pause");
      return -1;
    }

  cout << "machine: " << ip << endl;

  ODBST statinfo;
  memset ((void *) &statinfo, 0, sizeof (statinfo));
  ret = cnc_statinfo (h, &statinfo);

  if (EW_OK == ret)
    {
      cout << "AUTOMATIC/MANUAL mode selection \t:" << statinfo.aut << endl;
      cout << "Status of automatic operation \t\t: " << statinfo.run << endl;
      cout << "Status of axis movement, dwell \t\t: " << statinfo.motion <<
	endl;
      cout << "Status of M,S,T,B function \t\t: " << statinfo.mstb << endl;
      cout << "Status of alarm \t\t: " << statinfo.alarm << endl;
      cout << "Status of program editing \t\t: " << statinfo.edit << endl;
    }

  ODBSYS sysinfo;
  memset ((void *) &sysinfo, 0, sizeof (sysinfo));
  ret = cnc_sysinfo (h, &sysinfo);
  if (EW_OK == ret)
    {
      const int STR_MAX = 32;
      char cnc_type[STR_MAX] = { 0 };
      char mt_type[STR_MAX] = { 0 };
      char series[STR_MAX] = { 0 };
      char version[STR_MAX] = { 0 };
      memcpy (cnc_type, sysinfo.cnc_type, 2 * sizeof (char));
      memcpy (mt_type, sysinfo.mt_type, 2 * sizeof (char));
      memcpy (series, sysinfo.series, 4 * sizeof (char));
      memcpy (version, sysinfo.version, 4 * sizeof (char));
      cout << "cnc type:" << cnc_type << endl;
      cout << "mt type\t:" << mt_type << endl;
      cout << "series\t:" << series << endl;
      cout << "version\t:" << version << endl;
    }

  cout << "alarm===================" << endl;
  {
    ODBALMMSG msg[10];
    short rnum = 10;

    bool isOk = false;
    if (cnc_rdalmmsg ((unsigned short) h, -1, &rnum, msg) == EW_OK)
      {
	for (short i = 0; i < rnum; i++)
	  {
	    msg[i].alm_msg[31] = '\0';
	    cout << "ID_" << i << ": " << msg[i].alm_no << ", type: " <<
	      msg[i].type << endl;
	    cout << "Msg: " << msg[i].alm_msg << endl;
	    HexDump (msg[i].alm_msg, msg[i].msg_len, 0);
	  }
	isOk = true;
      }
  }

  cout << "test prog............." << endl;

  char ORG_NAME[] = "0221";
  char *prog_name = ORG_NAME;
  if (argc > 2)
    prog_name = argv[2];
  short code = cnc_delete (h, atoi (prog_name));
  printf ("Delete file: %s, code: %d\n", prog_name, code);
  //code = DownloadFile (h, prog_name);
  //printf ("download file: %s, code: %d\n", prog_name, code);
  string temp = prog_name;
  temp += ".nc";
  //code = UploadFile (h, atoi (prog_name), temp.c_str ());
  //printf ("upload file: %s, code: %d\n", prog_name, code);
  cnc_freelibhndl (h);


  cout << "end ..............:" << endl;
  return 0;
}

// short
// UploadFile (USHORT h, short progID, const char *filePath)
// {
//   short code = -1;
//   if (h > 0)
//     {
//       FILE *outfile = NULL;
//       outfile = fopen (filePath, "wb");
//       if (outfile != NULL)
// 	{
// 	  if (cnc_upstart3 (h, 0, progID, progID) == EW_OK)
// 	    {
// 	      long len = MAXLEN;
// 	      char buf[MAXLEN + 1];

// 	      do
// 		{
// 		  len = MAXLEN;
// 		  code = cnc_upload3 (h, &len, buf);
// 		  if (code == EW_BUFFER)
// 		    {
// 		      continue;
// 		    }
// 		  if (code == EW_OK)
// 		    {
// 		      buf[len] = '\0';
// 		      fwrite (buf, sizeof (char), len, outfile);
// 		    }
// 		  if (buf[len - 1] == '%')
// 		    {
// 		      break;
// 		    }
// 		}
// 	      while ((code == EW_OK) || (code == EW_BUFFER));
// 	      code = cnc_upend3 (h);
// 	    }
// 	  fclose (outfile);
// 	}
//     }
//   return code;
// }


// short
// DownloadFile (USHORT h, const char *filepath)
// {
//   short ret = -1;
//   if (h > 0)
//     {
//       FILE *readFile = fopen (filepath, "rb");
//       if (readFile != NULL)
// 	{
// 	  if (cnc_dwnstart3 (h, 0) == EW_OK)
// 	    {
// 	      long rc;
// 	      unsigned char buf[MAXLEN] = { 0 };
// 	      while (true)
// 		{
// 		  rc = fread (buf, sizeof (unsigned char), MAXLEN, readFile);
// 		  if (rc <= 0)
// 		    {
// 		      break;
// 		    }

// 		  do
// 		    {
// 		      ret = cnc_download3 (h, &rc, (char *) buf);
// 		    }
// 		  while (ret == EW_BUFFER);

// 		  if (ret != EW_OK)
// 		    {
// 		      break;
// 		    }
// 		}
// 	      ret = cnc_dwnend3 ((short) h);
// 	    }
// 	  fclose (readFile);
// 	}
//     }
//   return ret;
// }


void
HexDump (char *buf, int len, int addr)
{
  int i, j, k;
  char binstr[80];

  for (i = 0; i < len; i++)
    {
      if (0 == (i % 16))
	{
	  sprintf (binstr, "%08x -", i + addr);
	  sprintf (binstr, "%s %02x", binstr, (unsigned char) buf[i]);
	}
      else if (15 == (i % 16))
	{
	  sprintf (binstr, "%s %02x", binstr, (unsigned char) buf[i]);
	  sprintf (binstr, "%s ", binstr);
	  for (j = i - 15; j <= i; j++)
	    {
	      sprintf (binstr, "%s%c", binstr,
		       ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
	    }
	  printf ("%s\n", binstr);
	}
      else
	{
	  sprintf (binstr, "%s %02x", binstr, (unsigned char) buf[i]);
	}
    }
  if (0 != (i % 16))
    {
      k = 16 - (i % 16);
      for (j = 0; j < k; j++)
	{
	  sprintf (binstr, "%s ", binstr);
	}
      sprintf (binstr, "%s ", binstr);
      k = 16 - k;
      for (j = i - k; j < i; j++)
	{
	  sprintf (binstr, "%s%c", binstr,
		   ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
	}
      printf ("%s\n", binstr);
    }
}