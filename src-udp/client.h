/*
Author: Michael Andersson
Date : 2018-03-25

Functions in this file sets up and maintains the client side.
of the ring implementation based on Chang and Roberts algorithm.

The functions are fairly self explanatory.
*/

#ifndef __CLIENT
#define __CLIENT

#include "ringmain.h"
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

typedef struct {
  struct sockaddr serveraddr;
  socklen_t serveraddrLen;
  int socket_fd;
} serverInfo;
/*
* Sets up the connection to server using function arguments.
*/
serverInfo createUdpSocket(const char *remoteAdress, const int remotePort);

/*
* Prepares the next packet/message.
*/
void prepareMessage();

/*
* Checks the content of the incoming message, and will make a decision
* if or what message that should be forwarded in the ring.
* Return true if message should be forwarded/sent.
*/
bool checksContentOfIncomingMessage();

/*
* Decides which messages that should be forwarded in the ring.
*/
void forwardMessages(serverInfo sInfo);

void *clientMain(void *coliArg);

#endif
