#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h> 
#include <string.h> 
#ifndef __APPLE__
#include <wiringPi.h>
#endif

#include <stdlib.h>

#define MAX_MSG 100
#define OUTPUT_PIN 29

int main(int argc, char *argv[]) {
  
  int sd, rc, n, cliLen;
  struct sockaddr_in cliAddr, servAddr;
  char msg[MAX_MSG];
  int broadcast = 1;
  int camera, port;

  port = atoi(argv[1]);
  camera = atoi(argv[2]);
  printf("Listening on UDP port (%d) to Camera (%d)\n", port, camera);

#ifndef __APPLE__
  // Setup wiringPi
  if (wiringPiSetup () < 0)
    {
      perror ("Unable to setup wiringPi") ;
      exit(EXIT_FAILURE);
  }

  pinMode(OUTPUT_PIN, OUTPUT);
#endif

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
  servAddr.sin_port = htons(port);
  rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if(rc<0) {
    printf("%s: cannot bind port number %d \n", 
       argv[0], port);
    exit(1);
  }

  printf("%s: waiting for data on port UDP %u\n", 
       argv[0],port);

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

#ifndef __APPLE__
    if (argc > 1){
      if (msg[camera-1] == '1')
        digitalWrite (OUTPUT_PIN, HIGH);
      else
        digitalWrite (OUTPUT_PIN, LOW);
    }
#endif

  }

}

