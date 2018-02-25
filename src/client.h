#include "ringmain.h"
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "ringmain.h"

void clientMain(const char *remoteAdress, const int remotePort);

int setupConnectionToServer(const char *remoteAdress, const int remotePort);

void sendMessagesToServer(int client_fd);
