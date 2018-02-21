// TCP Implementation
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include "server.c"


typedef enum {
  ELECTION,
  ELECTION_OVER,
  MESSAGE
} phase;

typedef struct {
  char *localPort;
  char *remoteIP;
  char *remotePort;
} nodeArg;

int main(int argc, char **argv){
  nodeArg inputArg = 0;
  printf("IT IS WORKING !!!!");


  parseArgs(argc, argv, inputArg);
  setupServerConnection(inputArg);



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
