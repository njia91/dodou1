// TCP Implementation

#include "server.h"
#include "ringmain.h"
#include "client.h"

extern pthread_cond_t newMessage;
extern pthread_mutex_t mtxRingInfo;
extern ringInformation ringInfo;

struct addrinfo* fillinAddrInfo(const char* host, const int portNo,
                                struct addrinfo *hints){
  hints->ai_family=AF_UNSPEC;
  hints->ai_socktype=SOCK_STREAM;
  hints->ai_protocol=0;
  hints->ai_flags=AI_ADDRCONFIG;
  return hints;
}

void sigIntHandler(int sig){
	pthread_mutex_lock(&mtxRingInfo);
	ringInfo.ringActive = false;
	pthread_cond_broadcast(&newMessage);
	pthread_mutex_unlock(&mtxRingInfo);
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

    	fprintf(stderr, "%s\n", message);
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
		freeaddrinfo(info);
		return 2;
  }

  strncpy(fqdn, info->ai_canonname, n);
	freeaddrinfo(info);
  return 0;
}

int main(int argc, char **argv){
  nodeArg inputArg;
  fprintf(stdout, "Starting program\n"); 
  pthread_t serverThread;
  ringInfo.participant = false;

  pthread_mutex_init(&mtxRingInfo, NULL);
  pthread_cond_init(&newMessage, NULL);
	signal(SIGINT, sigIntHandler);

  char ownFQDN[256];
  if(getFQDN(ownFQDN, 256)){
    die("Could not retrieve node FQDN. Terminating\n");
  }

  ownFQDN[strlen(ownFQDN)] = ',';
  ringInfo.ownId = strncat(ownFQDN, argv[1], strlen(ownFQDN));
  ringInfo.ownId[strlen(ringInfo.ownId)+1] = '\n';
  ringInfo.highestId =  ringInfo.ownId;
  ringInfo.message = "Mian sending a message....\n";
  ringInfo.currentPhase = NOT_STARTED;
  ringInfo.ringActive = true;
	parseArgs(argc, argv, &inputArg);
  int ret = pthread_create(&serverThread, NULL, serverMain, (void *) &inputArg);
  if (ret){
    die(strerror(errno));
  }

  clientMain(inputArg.remoteIP, inputArg.remotePort);

  pthread_join(serverThread, NULL);
	pthread_mutex_destroy(&mtxRingInfo);
  pthread_cond_destroy(&newMessage);

	fprintf(stdout, "Ending Program\n");
}
