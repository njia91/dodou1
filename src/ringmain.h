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

static const char election_str[] = "ELECTION\n";
static const char election_over_str[] = "ELECTION_OVER\n";
static const char message_str[] = "MESSAGE\n";

typedef enum {
  NOT_STARTED,
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
  bool participant;
  char *message;
  char *highestId;
  char *ownId;
} ringInformation;

pthread_cond_t newMessage;
pthread_mutex_t mtxRingInfo;
ringInformation ringInfo;



//Error printing Function
void die(const char* message);

void parseArgs(int argc, char **argv, nodeArg *colArg);

int getFQDN(char *fqdn, size_t n);

struct addrinfo* fillinAddrInfo(const char* host, const int portNo,
                                struct addrinfo *hints);

#endif
