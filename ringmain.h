/**
Author: Michael Andersson
Date : 2018-03-25

Functions in this file sets up and maintains the server side.
of the ring implementation based on Chang and Roberts algorithm.

The functions are fairly self-explanatory.

**/
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
  char *message;
} nodeArg;

// Used for client and server communication.
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

/*
* Parses argument from command line.
*/
void parseArgs(int argc, char **argv, nodeArg *colArg);

/*
* Gets the fully qualified domain name (FQDN) for the computer.
*/
int getFQDN(char *fqdn, size_t n);

/*
* Fills in addrInfo struct with common info for both client and server.
*/
void fillinAddrInfo(struct addrinfo **result, const int localPort,
                    const char* IPAdress, int flags);

#endif
