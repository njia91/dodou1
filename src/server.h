#ifndef __SERVER
#define __SERVER

#include "ringmain.h"
#include <stdlib.h>

void prepareMessage();
int setupServerConnection(const int localPort);
int listenForIncommingConnection(int server_fd);
void handleConnectionSession(int connection_fd);
void *serverMain(void *localPort);

#endif
