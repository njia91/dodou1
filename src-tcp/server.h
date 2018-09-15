/**
Author: Michael Andersson
Date : 2018-03-25

Functions in this file sets up and maintains the server side.
of the ring implementation based on Chang and Roberts algorithm.

The functions are fairly self-explanatory.
**/


#ifndef __SERVER
#define __SERVER

#include "ringmain.h"
#include <stdlib.h>
#include "common.h"


/*
* Listening for incoming connections attempts.
*/
int listenForIncommingConnection(int server_fd);

/*
* Creates the socket using function arguments.
*/
int setupServerConnection(const int localPort);

/*
* Handles the connections and checks the socket specified
* by function argument(connection_fd).
*/
void handleConnectionSession(int connection_fd);

void serverMain(int localPort);

#endif
