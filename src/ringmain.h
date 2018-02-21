#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
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
  char *localPort,
  char *remoteIP,
  char *remotePort,
} nodeArg;

typedef struct {
  phase currentPhase,
  char *message,
  char *highestId;
  char *ownId;
} ringInformation;
