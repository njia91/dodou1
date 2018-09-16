#include "UDPclient.h"




void forwardMessages(serverInfo sInfo){
  bool active = true;
  bool shortSleep = false;
  char packetToSend[100];
  while(active){
    pthread_mutex_lock(&mtxRingInfo);
    if(shouldMessageBeForwarded(true)){
      memset(packetToSend, '\0', PACKET_SIZE);
      prepareMessage(packetToSend);
      if((sendto(sInfo.socket_fd, packetToSend, PACKET_SIZE, 0,
                  (struct sockaddr*) &sInfo.serveraddr, sInfo.serveraddrLen)) == -1){
        fprintf(stderr, "Something is wrong with the socket/Connection: %s \n", strerror(errno));
        ringInfo.ringActive = false;
      }
    }

    if(ringInfo.ringActive && (ringInfo.currentPhase == NOT_STARTED || ringInfo.currentPhase == ELECTION)){
      shortSleep = true;
    } else {
      pthread_cond_wait(&newMessage,  &mtxRingInfo);
    }

    if(!ringInfo.ringActive){
      active = false;
		}
    pthread_mutex_unlock(&mtxRingInfo);
    // Before MESSAGE phase, make short sleeps to avoid spam.
    if (shortSleep &&  ringInfo.ringActive){
      sleep(3);
    }
    shortSleep = false;
  }
  printf("Terminating Client\n");
}

serverInfo createUdpSocket(const char *remoteAdress, const int remotePort){
  int client_fd;
  char portId[15];
  sprintf(portId, "%d", remotePort);
  struct addrinfo* result=0;
  fillinAddrInfo(&result, remotePort, remoteAdress, AI_ADDRCONFIG);

  client_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (client_fd ==-1){
    freeaddrinfo(result);
    die(strerror(errno));
  }
  serverInfo sInfo;
  sInfo.serveraddr = *result->ai_addr;
  sInfo.serveraddrLen = result->ai_addrlen;
  sInfo.socket_fd = client_fd;
  freeaddrinfo(result);
  return sInfo;
}

void *clientMain(void *coliArg){
  nodeArg args = * (nodeArg *) coliArg;
  serverInfo sInfo = createUdpSocket(args.remoteIP, args.remotePort);

  if (sInfo.socket_fd != -1){
    forwardMessages(sInfo);
  }
  close(sInfo.socket_fd);
  pthread_exit(NULL);
}
