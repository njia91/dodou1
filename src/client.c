#include "client.h"

void prepareMessage(char *packetToSend){
    switch(ringInfo.currentPhase){
      case NOT_STARTED:
        strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR));
        strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
        break;
      case ELECTION:
        strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR));
        strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
        break;
      case ELECTION_OVER:
        strncpy(packetToSend, ELECTION_OVER_STR, strlen(ELECTION_OVER_STR));
        strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
      	break;
      case MESSAGE:
      	strncpy(packetToSend, MESSAGE_STR, strlen(MESSAGE_STR));
      	strncat(packetToSend, ringInfo.message, strlen(ringInfo.message));
        break;
      default:
				fprintf(stderr, "Invalid phase, should not happen\n");
        ringInfo.ringActive = false;
        break;
    }
}

void sendMessagesToServer(int client_fd){
  bool active = true;
  int ret;
  char packetToSend[100];
  bool shouldMessageBeForwarded = false;
  while(ringInfo.ringActive){
    pthread_mutex_lock(&mtxRingInfo);
    
		switch(ringInfo.currentPhase){
      case NOT_STARTED:
        ringInfo.participant = false;
        shouldMessageBeForwarded = true;
        break;
      case ELECTION:
        ret = strcmp(ringInfo.receivedMessage, ringInfo.highestId);
        if(ret > 0 ){
          shouldMessageBeForwarded = true;
          ringInfo.participant = true;
          ringInfo.highestId = ringInfo.receivedMessage;
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
        }
        shouldMessageBeForwarded = true;
        break;
      case MESSAGE:
				ringInfo.message = ringInfo.receivedMessage;
      	shouldMessageBeForwarded = true;
				break;
      default:
		 		fprintf(stderr, "client.cc - Unknown phase - Should not happen.\n");
        ringInfo.ringActive = false;
				break;
    }

    if(shouldMessageBeForwarded){
        memset(packetToSend, 0, PACKET_SIZE);
        prepareMessage(packetToSend);
        if((send(client_fd, packetToSend, sizeof(packetToSend), 0)) == -1){
					fprintf(stderr, "Something is wrong with the socket: %s ", strerror(errno));
					active = false;
				}
    }

		if(active){
    	memset(ringInfo.receivedMessage, 0, sizeof(ringInfo.receivedMessage));
    	pthread_cond_wait(&newMessage, &mtxRingInfo);
    	pthread_mutex_unlock(&mtxRingInfo);
			//sleep(2);
		}
  }
}

int setupConnectionToServer(const char *remoteAdress, const int remotePort){
  int client_fd;
  char portId[15];
  sprintf(portId, "%d", remotePort);
  struct addrinfo hints;
  memset(&hints,0,sizeof(hints));

  fillinAddrInfo(remoteAdress, remotePort, &hints);
  struct addrinfo* result=0;

  int err = getaddrinfo(remoteAdress, portId,&hints,&result);
  if (err != 0) {
      die(gai_strerror(err));
  }

  client_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (client_fd ==-1){
    die(strerror(errno));
  }

  while(connect(client_fd, result->ai_addr, result->ai_addrlen) == -1){
    fprintf(stderr, "Unable to connect - Will retry to connect: "
                    "Errno: %s \n", strerror(errno));
    sleep(2);
  }


  freeaddrinfo(result);

  return client_fd;

}

void clientMain(const char *remoteAdress, const int remotePort){
  int client_fd = setupConnectionToServer(remoteAdress, remotePort);
  sendMessagesToServer(client_fd);
}

