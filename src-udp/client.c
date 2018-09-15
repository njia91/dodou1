#include "client.h"

void prepareMessage(char *packetToSend){
  switch(ringInfo.currentPhase){
    case NOT_STARTED:
      strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR) + 1);
      strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
      break;
    case ELECTION:
      strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR) + 1);
      strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
      break;
    case ELECTION_OVER:
      strncpy(packetToSend, ELECTION_OVER_STR, strlen(ELECTION_OVER_STR) + 1);
      strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
      break;
    case MESSAGE:
      strncpy(packetToSend, MESSAGE_STR, strlen(MESSAGE_STR) + 1);
      strncat(packetToSend, ringInfo.message, strlen(ringInfo.message));
      break;
    default:
      fprintf(stderr, "Invalid phase, should not happen\n");
      ringInfo.ringActive = false;
      break;
  }
}

bool checksContentOfIncomingMessage(){
  bool shouldMessageBeForwarded = false;
  int ret;
  switch(ringInfo.currentPhase){
    case NOT_STARTED:
      ringInfo.participant = false;
      shouldMessageBeForwarded = true;
      break;
    case ELECTION:
      ret = strcmp(ringInfo.receivedMessage, ringInfo.highestId);
      if(ret > 0 ){
        ringInfo.participant = true;
        ringInfo.highestId = ringInfo.receivedMessage;
        shouldMessageBeForwarded = true;
      }
      else if (ret < 0 && ringInfo.participant == true){
        shouldMessageBeForwarded = false;
      }
      else if ( ret < 0 && ringInfo.participant == false){
        shouldMessageBeForwarded = true;
        ringInfo.participant = true;
      }
      else if ( ret == 0){
        ringInfo.currentPhase = ELECTION_OVER;
        ringInfo.participant = false;
        shouldMessageBeForwarded = true;
      }
      break;
    case ELECTION_OVER:
      if ( strcmp(ringInfo.receivedMessage, ringInfo.ownId) == 0){
        ringInfo.currentPhase = MESSAGE;
        ringInfo.ringLeader = true;
        ringInfo.startTime = clock();

      }
      shouldMessageBeForwarded = true;
      break;
    case MESSAGE:
      ringInfo.message = ringInfo.receivedMessage;
      shouldMessageBeForwarded = true;
      break;
    default:
      fprintf(stderr, "client.cc: Unknown phase - Should not happen.\n");
      ringInfo.ringActive = false;
      break;
  }
  return shouldMessageBeForwarded;
}

void forwardMessages(serverInfo sInfo){
  bool active = true;
  char packetToSend[100];
  while(active){
    pthread_mutex_lock(&mtxRingInfo);
    if(checksContentOfIncomingMessage()){
        memset(packetToSend, '\0', PACKET_SIZE);
        prepareMessage(packetToSend);
        if((sendto(sInfo.socket_fd, packetToSend, PACKET_SIZE, 0,
                  (struct sockaddr*) &sInfo.serveraddr, sInfo.serveraddrLen)) == -1){
            fprintf(stderr, "Something is wrong with the socket/Connection: %s \n", strerror(errno));
            ringInfo.ringActive = false;
        }
    }
    printf("Seending packages UDP\n");

    if(ringInfo.ringActive && !(ringInfo.currentPhase == NOT_STARTED || ringInfo.currentPhase == ELECTION)){
      pthread_cond_wait(&newMessage,  &mtxRingInfo);
    }

    if(!ringInfo.ringActive){
      active = false;
		}
    pthread_mutex_unlock(&mtxRingInfo);


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
