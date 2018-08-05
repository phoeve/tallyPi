#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h> 
#include <string.h> 
#include <wiringPi.h>

#include <stdlib.h>

#define LOCAL_SERVER_PORT 1500
#define MAX_MSG 100
#define OUTPUT_PIN 29

int main(int argc, char *argv[]) {
  
  int sd, rc, n, cliLen;
  struct sockaddr_in cliAddr, servAddr;
  char msg[MAX_MSG];
  int broadcast = 1;



  printf("Listening to Camera %s Tally\n", argv[1]);

  // Setup wiringPi
  if (wiringPiSetup () < 0)
    {
      perror ("Unable to setup wiringPi") ;
      exit(EXIT_FAILURE);
  }

  pinMode(OUTPUT_PIN, OUTPUT);

  /* socket creation */
  sd=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd<0) {
    printf("%s: cannot open socket \n",argv[0]);
    exit(1);
  }
  
  if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) {
          perror("setsockopt (SO_BROADCAST)");
          exit(1);
  }

  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(LOCAL_SERVER_PORT);
  rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if(rc<0) {
    printf("%s: cannot bind port number %d \n", 
       argv[0], LOCAL_SERVER_PORT);
    exit(1);
  }

  printf("%s: waiting for data on port UDP %u\n", 
       argv[0],LOCAL_SERVER_PORT);

  while(1) {
    
    memset(msg,0x0,MAX_MSG);


    cliLen = sizeof(cliAddr);
    n = recvfrom(sd, msg, MAX_MSG, 0, 
         (struct sockaddr *) &cliAddr, &cliLen);

    if(n<0) {
      printf("%s: cannot receive data \n",argv[0]);
      continue;
    }
      
    printf("%s: from %s:UDP%u : %s \n", 
       argv[0],inet_ntoa(cliAddr.sin_addr),
       ntohs(cliAddr.sin_port),msg);

	if (argc > 1){
		if (msg[atoi(argv[1])-1] == '1')
			digitalWrite (OUTPUT_PIN, HIGH);
		else
			digitalWrite (OUTPUT_PIN, LOW);
	}

  }

}

