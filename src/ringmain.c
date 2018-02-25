// TCP Implementation

#include "server.h"
#include "ringmain.h"
#include "client.h"

pthread_cond_t newMessage;
pthread_mutex_t mtxRingInfo;
ringInformation ringInfo;

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

int getFQDN(char *fqdn, size_t n){
  char hostname[256];
  int r = gethostname(hostname, 256);

  if(r != 0){
    return 1;
  }
  struct addrinfo h = {0};
  h.ai_family = AF_INET;
  h.ai_socktype = SOCK_STREAM;
  h.ai_flags = AI_CANONNAME;

  struct addrinfo *info;
  if(getaddrinfo(hostname, NULL, &h, &info) != 0){
    return 2;
  }

  strncpy(fqdn, info->ai_canonname, n);
  freeaddrinfo(info);

  return 0;
}

int main(int argc, char **argv){
  nodeArg inputArg;
  printf("Start of main!!!!%lu \n", pthread_self());
  pthread_t serverThread;
  ringInfo.participant = false;

  pthread_mutex_init(&mtxRingInfo, NULL);
  pthread_cond_init(&newMessage, NULL);

  char ownFQDN[256];
  if(getFQDN(ownFQDN, 256)){
    die("Could not retrieve node FQDN. Terminating\n");
  }

  ownFQDN[strlen(ownFQDN)] = ',';
  ringInfo.ownId = strncat(ownFQDN, argv[1], strlen(ownFQDN));
  ringInfo.highestId =  ringInfo.ownId;
  printf("RING ID: %s \n",ringInfo.ownId);
  ringInfo.currentPhase = NOT_STARTED;
//printf("NOT STARTED222!!!!!! %d \n", ringInfo.currentPhase);
  parseArgs(argc, argv, &inputArg);
//printf("NOT STARTED222!!!!!! %d \n", ringInfo.currentPhase);
  int ret = pthread_create(&serverThread, NULL, serverMain, (void *) &inputArg.localPort);
  if (ret){
    die(strerror(errno));
  }

  //Connection to server.
  clientMain(inputArg.remoteIP, inputArg.remotePort);

  pthread_mutex_destroy(&mtxRingInfo);
  pthread_cond_destroy(&newMessage);
  //Recieve Packet
}
