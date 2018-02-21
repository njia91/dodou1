// TCP Implementation
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>

#include "server.c"
#include "ringmain.h"

pthread_cond_t newMessage;
pthread_mutex_t mtxRingInfo;


int main(int argc, char **argv){
  nodeArg inputArg = 0;
  printf("IT IS WORKING !!!!");

  pthread_mutex_init(&mtxRingInfo, NULL);
  pthread_cond_init(&nerMessage, NULL);

  parseArgs(argc, argv, inputArg);

  serverMain(inputArg->localPort);

  //Connection to server.

  //Recieve Packet
}

struct addrinfo* fillinAddrInfo(const char* host, const char *portNo){
  const char* hostname = host; /* localhost */
  const char* portname = portNo;
  struct addrinfo hints;
  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_protocol=0;
  hints.ai_flags=AI_ADDRCONFIG;
  return hints;
}

//Error printing Function
void die(char* message){
    	fprintf(stderr, message);
    	exit(EXIT_FAILURE);
}

void parseArgs(int argc, char **argv, nodeArg *inputArg){
  if (argc < 4){
    die("Too few Arguments \n
          <ProgramName> [localPort] [remote IP Adress] [remote Port]\n")
  }
  arg->localPort = argv[1];
  arg->remoteIP = argv[2];
  arg->remotePort = argv[3];
}
