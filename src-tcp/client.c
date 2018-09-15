#include "client.h"

void forwardMessages(int client_fd){
  bool active = true;
  char packetToSend[100];
  while (active){
    pthread_mutex_lock(&mtxRingInfo);
    if (shouldMessageBeForwarded(false)) {
      // Repackage the packet and send message.
      memset(packetToSend, '\0', PACKET_SIZE);
      prepareMessage(packetToSend);
      if ((send(client_fd, packetToSend, PACKET_SIZE, 0)) == -1) {
        fprintf(stderr, "Something is wrong with the socket: %s \n", strerror(errno));
      }
    }
		if (ringInfo.ringActive){
      pthread_cond_wait(&newMessage, &mtxRingInfo);
		} else {
      active = false;
    }
    pthread_mutex_unlock(&mtxRingInfo);
  }
  printf("Terminating Client\n");
}

int setupConnectionToServer(const char *remoteAdress, const int remotePort){
  int client_fd;
  char portId[15];

  // Convert Port to String
  sprintf(portId, "%d", remotePort);
  struct addrinfo* result=0;
  fillinAddrInfo(&result, remotePort, remoteAdress, AI_ADDRCONFIG);

  client_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (client_fd ==-1){
    freeaddrinfo(result);
    die(strerror(errno));
  }

  // Used to prioritize packets from this socket.
  int optval = 6;
  if (setsockopt(client_fd, SOL_SOCKET, SO_PRIORITY, &optval, sizeof(int)) == -1){
    close(client_fd);
    die(strerror(errno));
  }

  // Try to connect to Server. Retry if unsuccessful.
  while(connect(client_fd, result->ai_addr, result->ai_addrlen) == -1){
    fprintf(stderr, "Unable to connect - Will retry to connect. "
                    "Errno: %s \n", strerror(errno));

    pthread_mutex_lock(&mtxRingInfo);
    //If ring is broken, do not bother to reconnect
    if (ringInfo.ringActive == false){
      client_fd = -1;
    }
    pthread_mutex_unlock(&mtxRingInfo);
    sleep(3);
  }
  freeaddrinfo(result);
  return client_fd;
}

void *clientMain(void *coliArg){
  nodeArg args = * (nodeArg *) coliArg;
  int client_fd = setupConnectionToServer(args.remoteIP, args.remotePort);

  if (client_fd != -1){
    forwardMessages(client_fd);
  }
  close(client_fd);
  pthread_exit(NULL);
}
