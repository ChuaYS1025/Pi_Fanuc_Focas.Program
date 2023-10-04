#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "../external/fwlib/fwlib32.h"
#define Max_Digits 10
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <curl/curl.h>
#pragma comment(lib,"libcurl.lib")

short unsigned int libh;  // fwlib handle
//struct timeval ts;

int rdparam(short num, IODBPSD *param) {
  short ret;
  ret = cnc_rdparam(libh, num, ALL_AXES, sizeof(*param), param);
  if (ret != EW_OK) {
    fprintf(stderr, "Failed to read parameter %d: %d!\n", num, ret);
    return 1;
  }
  return 0;
}

void cleanup() {
  if (cnc_freelibhndl(libh) != EW_OK)
    fprintf(stderr, "Failed to free library handle!\n");
  cnc_exitprocess();
}
int http_post(const char *url, const char *data) { 
    CURL *curl;
    CURLcode res = CURLE_OK;

    // Initialize cURL
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize cURL\n");
        return -1;
    }

    // Set the URL to send the POST request to
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Set the POST data
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    // Perform the POST request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return -1;
    }

    // Cleanup cURL handle
    curl_easy_cleanup(curl);

    return 0;
}


int main(int argc, char **argv) {
  
  int device_port = 8193; /* typical FOCAS port */
  char device_ip[40] = "192.168.1.1";
  char cnc_id[36];
  char axes_names[MAX_AXIS * 2] = "";
  short axis_count = MAX_AXIS;
  
  unsigned long cnc_ids[4];
  IODBPSD param = {0}; /* reset param result memory, not set */
  ODBST statinfo;
  ODBAXISNAME axes[MAX_AXIS] = {{0}};
  int partcnt;
  int previouspartcnt =0;
  int previousalarmsignal =0;
  struct timeval ts;
  //struct timespec ts;
  //time_t ts;
  int timestamp;
  int t1;
  int t3;
  char t4[Max_Digits + sizeof(char)];
  
  
  
  if (argc < 2) {
    fprintf(stderr, "%% Usage: %s <ip> <port>\n", argv[0]);
    return 1;
  }
  if (argc >= 3) {
    sscanf(argv[2], "%d", &device_port);
  }
  sscanf(argv[1], "%s", device_ip);

  if (cnc_startupprocess(0, "focas.log") != EW_OK) {
   fprintf(stderr, "Failed to create required log file!\n");
    return 1;
  }

  printf("Connecting to %s:%d\n", device_ip, device_port);

  // library handle.  needs to be closed when finished.
  if (cnc_allclibhndl3(device_ip, device_port, 10 /* timeout */, &libh) != EW_OK)
   {
        fprintf(stderr, "Failed to connect to cnc!\n");
        
        return 1;
  }
  //atexit(cleanup); 
  
			if (rdparam(6711, &param)) // read parameter (part count)
			{
				fprintf(stderr, "Failed to get part count!\n");
        return 1;
			}
			printf("part count (%d): %ld\n", param.datano, param.u.ldata);
			partcnt = param.u.ldata;
      
      //timestamp
      //gettimeofday(&ts,NULL);
      //int t1 = (ts.tv_sec);
      //int t2 = ts.tv_usec;
      //int t3 = ts.tv_usec/1000;
      //printf("sec time: %ld\n",t1);
      //printf("msec time: %ld\n",t2);
      //printf("msec time: %ld\n",t3);
      //char t4[Max_Digits + sizeof(char)];
      //sprintf(t4,"%ld%ld",t1,t3);
      //printf("totaltime: %s\n",t4);
      //// http coding
      //http post
    

      /////
      while (1)
      {
        //timestamp
       // gettimeofday(&ts,NULL);
        // t1 = (ts.tv_sec);
        //int t2 = ts.tv_usec;
        // t3 = ts.tv_usec/1000;
        
       // sprintf(t4,"%ld%ld",t1,t3);
        //printf("totaltime: %s\n",t4);
        
        if (rdparam(6711, &param)) // read parameter (part count)
			{
				fprintf(stderr, "Failed to get part count!\n");
        
				return 1;
			}
			
			//printf("part count (%d): %ld\n", param.datano, param.u.ldata);
			int partcnt = param.u.ldata;
      //printf("OUTPUT: %d\n",partcnt);
      
			if (partcnt != previouspartcnt)
			{
				/*char *filename2;
        sprintf( filename2, "/home/pi/Desktop/logfile/OUTPUT1/OUTPUT1_%s.txt", t4 );

        // open the file for writing
				FILE *fp = fopen(filename2, "w+");
				if (fp == NULL)
				{
					printf("Error opening the file %s", filename2);
					return -1;
				}
				// write to the text file
				fprintf(fp, "{'MACHINEID':MC144,'OUTPUT':%ld,'TIMESTAMP':%s}", partcnt,t4);

				// close the file
        // close the file
				fclose(fp);*/
				//printf("FILENAME:%s",filename2);
        // Construct POST data
            char post_data[256];
            snprintf(post_data, sizeof(post_data), "MACHINEID=MC144&OUTPUT=%ld", partcnt);

            // Send HTTP POST request using cURL
            if (http_post("http://127.0.0.1:1880/OUTPUT1", post_data) != 0) {
                fprintf(stderr, "HTTP POST failed\n");
            }

				previouspartcnt = partcnt;
        printf("DONE\n");
        printf("OUTPUT:%ld\n",partcnt);
			}
			
      
      if (cnc_statinfo(libh, &statinfo) != EW_OK)
			{
				fprintf(stderr, "Failed to get cnc info!\n");
        
        
				return 1;
			}
			int alarmsignal = statinfo.alarm;
			if (alarmsignal != previousalarmsignal)
			{
				
				/*char *filename2;
        sprintf( filename2, "/home/pi/Desktop/logfile/DOWNSTATUS/DOWNSTATUS_%s.txt", t4 );

				// open the file for writing
				FILE *fp = fopen(filename2, "w+");
				if (fp == NULL)
				{
					printf("Error opening the file %s", filename2);
					return -1;
				}
				// write to the text file
				fprintf(fp, "{'MACHINEID':MC144,'STATUS':%ld,'TIMESTAMP':%s}", alarmsignal,t4);

				// close the file
				fclose(fp);*/
        // Construct POST data
            char post_data[256];
            snprintf(post_data, sizeof(post_data), "MACHINEID=MC144&STATUS=%ld", alarmsignal);

            // Send HTTP POST request using cURL
            if (http_post("http://127.0.0.1:1880/DOWNSTATUS", post_data) != 0) {
                fprintf(stderr, "HTTP POST failed\n");
            }

				previousalarmsignal = alarmsignal;
				
			}
      
        
        char post_data[256];
            snprintf(post_data, sizeof(post_data), "STATUS=ALIVE");

            // Send HTTP POST request using cURL
            if (http_post("http://127.0.0.1:1880/ALIVE", post_data) != 0) {
                fprintf(stderr, "HTTP POST failed\n");
            }
        //fprintf(stderr, "ALIVE\n");
        sleep(2);
        
        }
    
  
 
  
  
  
    
  
  return 0;
  
  
}
