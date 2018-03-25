#ifndef __RING
#define __RING
#define _DEFAULT_SOURCE

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>

static const char ELECTION_STR[] = "ELECTION\n";
static const char ELECTION_OVER_STR[] = "ELECTION_OVER\n";
static const char MESSAGE_STR[] = "MESSAGE\n";
static const int PACKET_SIZE = 100;

typedef enum {
  ELECTION,
  ELECTION_OVER,
  MESSAGE,
  NOT_STARTED
} phase;

typedef struct {
  int localPort;
  char *remoteIP;
  int remotePort;
} nodeArg;

typedef struct {
  phase currentPhase;
  bool participant;
	bool ringActive;
	char *message;
  char receivedMessage[100];
  char *highestId;
  char *ownId;
} ringInformation;

pthread_cond_t newMessage;
pthread_mutex_t mtxRingInfo;
ringInformation ringInfo;


//Error printing Function
void die(const char* message);

void parseArgs(int argc, char **argv, nodeArg *colArg);


void sigIntHandler(int sig);

int getFQDN(char *fqdn, size_t n);

struct addrinfo* fillinAddrInfo(const char* host, const int portNo,
                                struct addrinfo *hints);

#endif
