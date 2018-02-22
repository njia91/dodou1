// TCP Implementation

#include "server.h"
#include "ringmain.h"
#include "client.h"

pthread_cond_t newMessage;
pthread_mutex_t mtxRingInfo;

struct addrinfo* fillinAddrInfo(const char* host, const int portNo,
                                struct addrinfo *hints){
  hints->ai_family=AF_UNSPEC;
  hints->ai_socktype=SOCK_STREAM;
  hints->ai_protocol=0;
  hints->ai_flags=AI_ADDRCONFIG;
  return hints;
}



void parseArgs(int argc, char **argv, nodeArg *colArg){
  if (argc < 4){
    die("Too few Arguments \n"
          "<ProgramName> [localPort] [remote IP Adress] [remote Port]\n");
  }
  colArg->localPort = htons(atoi(argv[1]));
  colArg->remoteIP = argv[2];
  colArg->remotePort = htons(atoi(argv[3]));
}

void die(const char* message){

    	fprintf(stderr, "What is failing... %s", message);
    	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
  nodeArg inputArg;
  printf("Start of main!!!!%lu \n", pthread_self());
  pthread_t serverThread;


  pthread_mutex_init(&mtxRingInfo, NULL);
  pthread_cond_init(&newMessage, NULL);

  parseArgs(argc, argv, &inputArg);

  int ret = pthread_create(&serverThread, NULL, serverMain, (void *) &inputArg.localPort);
  if (ret){
    die(strerror(errno));
  }
  //serverMain(inputArg.localPort);

  //Connection to server.
  clientMain(inputArg.remoteIP, inputArg.remotePort);

  //Recieve Packet
}
