#include "client.h"


void clientMain(const char *remoteAdress, const int remotePort){
  int client_fd = setupConnectionToServer(remoteAdress, remotePort);
  sendMessagesToServer(client_fd);
}

void prepareMessage(char *packetToSend){
    switch(ringInfo.currentPhase){
      case NOT_STARTED:
        strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR));
        strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("ELECTION:  %s \n", packetToSend);
        break;
      case ELECTION:
        strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR));
        strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("ELECTION:  %s \n", packetToSend);
        break;
      case ELECTION_OVER:
        strncpy(packetToSend, ELECTION_OVER_STR, strlen(ELECTION_OVER_STR));
        strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("ELECTION_OVER %s \n",packetToSend);
      	break;
      case MESSAGE:
      strncpy(packetToSend, MESSAGE_STR, strlen(MESSAGE_STR));
      strncat(packetToSend, ringInfo.message, strlen(ringInfo.message));
      printf("MESSAGE  %s \n",packetToSend);
        break;
      default:
        die("Something is wrong.. \n");
        break;
    }
}

void sendMessagesToServer(int client_fd){
  bool active = true;
  int count = 0;
  int ret;
  char packetToSend[100];
  bool shouldMessageBeForwarded = false;
  while(active){
    count++;
	  printf("BEGINING OF LOOP\n");
    pthread_mutex_lock(&mtxRingInfo);
    // Server will wake upp client when server is setup
    printf("THIS MESSAGE ARRIVED...!!!%s \n", ringInfo.receivedMessage);
    //Handle incoming messaege different depending on current phase.
    switch(ringInfo.currentPhase){
      case NOT_STARTED:
        ringInfo.participant = false;
        shouldMessageBeForwarded = true;
        break;
      case ELECTION:
        ret = strcmp(ringInfo.receivedMessage, ringInfo.highestId);
        if(ret > 0 ){
          printf("ID BIFFER THEN OTHER...!!!%s \n", ringInfo.receivedMessage);
          shouldMessageBeForwarded = true;
          ringInfo.participant = true;
          ringInfo.highestId = ringInfo.receivedMessage;
        }
        else if (ret < 0 && ringInfo.participant == true){
          shouldMessageBeForwarded = false;
          printf("THIS IF SATETMET...!!!%sParticitand %d\n", ringInfo.receivedMessage, (int)ringInfo.participant);
        }
        else if ( ret < 0 && ringInfo.participant == false){
          shouldMessageBeForwarded = true;
          ringInfo.participant = true;
          printf("THIS IF SATETMET123123...!!!%s  Particitand %d\n", ringInfo.receivedMessage, (int)ringInfo.participant);
        }
        else if ( ret == 0){
          printf("message phase entered!!!%s \n", ringInfo.receivedMessage);
          ringInfo.currentPhase = ELECTION_OVER;
          ringInfo.participant = false;
          shouldMessageBeForwarded = true;
        }
        break;
      case ELECTION_OVER:
      //DO NOTHING?
        if ( strcmp(ringInfo.receivedMessage, ringInfo.ownId) == 0){
          printf("ELECTION_OVER goes to  Message phase!!!%s \n", ringInfo.receivedMessage);
          ringInfo.currentPhase = MESSAGE;
        }
        shouldMessageBeForwarded = true;
        break;
      case MESSAGE:
      printf("MESSAGE STAGE!! !%s \n", ringInfo.receivedMessage);
      ringInfo.message = ringInfo.receivedMessage;
      shouldMessageBeForwarded = true;
      default:
      //  die("Something is wrong.. \n");
        break;
    }

    if(shouldMessageBeForwarded){
        memset(packetToSend, 0, PACKET_SIZE);
        prepareMessage(packetToSend);
        send(client_fd, packetToSend, sizeof(packetToSend), 0);
    }

    printf("Going to sleep.... %d\n", count);
    memset(ringInfo.receivedMessage, 0, sizeof(ringInfo.receivedMessage));
    pthread_cond_wait(&newMessage, &mtxRingInfo);
    pthread_mutex_unlock(&mtxRingInfo);
    sleep(4);
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
  if (err!=0) {
      die(gai_strerror(err));
  }

  client_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (client_fd ==-1){
    die(strerror(errno));
  }

  while(connect(client_fd, result->ai_addr, result->ai_addrlen) == -1){
    fprintf(stderr, "Unable to connect - Will retry to connect."
                    "Errno: %s \n", strerror(errno));
    sleep(2);
  }


  freeaddrinfo(result);

  return client_fd;

}
