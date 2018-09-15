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
#include "common.h"

/*
* Sets up the connection to server using function arguments.
*/
int setupConnectionToServer(const char *remoteAdress, const int remotePort);

/*
* Decides which messages that should be forwarded in the ring.
*/
void forwardMessages(int client_fd);

void *clientMain(void *coliArg);

#endif
