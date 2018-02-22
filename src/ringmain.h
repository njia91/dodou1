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

pthread_cond_t newMessage;
pthread_mutex_t mtxRingInfo;


typedef enum {
  ELECTION,
  ELECTION_OVER,
  MESSAGE
} phase;

typedef struct {
  int localPort;
  char *remoteIP;
  int remotePort;
} nodeArg;

typedef struct {
  phase currentPhase;
  char *message;
  char *highestId;
  char *ownId;
} ringInformation;

//Error printing Function
void die(const char* message);

void parseArgs(int argc, char **argv, nodeArg *colArg);

struct addrinfo* fillinAddrInfo(const char* host, const int portNo,
                                struct addrinfo *hints);

#endif
