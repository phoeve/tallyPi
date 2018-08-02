#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <syslog.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */ 
#include <wiringPi.h>

#include <stdlib.h>

#define REMOTE_SERVER_PORT 1500

int main(int argc, char *argv[]) {
  
  unsigned char gpio_pin[] = {0,1,2,3,4,5,6};
  int i, sd, rc;
  struct sockaddr_in cliAddr, remoteServAddr;
  struct hostent *h;
  int broadcast = 1;
  char buffer[128];


    setlogmask (LOG_UPTO (LOG_INFO));
    openlog ("tallyServer", LOG_PID, LOG_USER);

    // Setup wiringPi
    if (wiringPiSetup () < 0)
    {
        perror ("Unable to setup wiringPi") ;
        exit(EXIT_FAILURE);
    }

    //init gpio pin values
    for (i = 0; i < sizeof(gpio_pin); i++)
    {
        pinMode(gpio_pin[i], INPUT);
        pullUpDnControl(gpio_pin[i], PUD_UP);
    }
  
  h = gethostbyname("255.255.255.255");

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr, 
     h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    perror("cannot open socket");
    exit(1);
  }
  
  if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) {
    perror("setsockopt (SO_BROADCAST)");
    exit(1);
  }
  
  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);
  
  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    perror("cannot bind port");
    exit(1);
  }


    while(1){
        for (i=0; i<sizeof(gpio_pin); i++){
            buffer[i] = (!digitalRead(gpio_pin[i])+48);
        }

	  rc = sendto(sd, buffer, sizeof(gpio_pin), 0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
	  if(rc<0) {
	    printf("cannot send data\n");
	    close(sd);
	    exit(1);
	  }
		usleep(250000);
    }

  
  return 1;

}

