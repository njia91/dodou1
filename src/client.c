#include "client.h"


void clientMain(const char *remoteAdress, const int remotePort){
  int client_fd = setupConnectionToServer(remoteAdress, remotePort);
  sendMessagesToServer(client_fd);
}

void sendMessagesToServer(int client_fd){
  bool active = true;
  int count = 0;
  char message[100] = "\0";
  // if(ringInfo.currentPhase == NOT_STARTED){
  //   sleep(1);
  // }

  while(active){
        count++;
	  printf("BEGINING OF LOOP\n");
    memset(&message, 0, sizeof(message));
    pthread_mutex_lock(&mtxRingInfo);
    switch(ringInfo.currentPhase){
      case NOT_STARTED:
        strncpy(message, election_str, strlen(election_str));
        strncat(message, ringInfo.ownId, strlen(ringInfo.ownId));
        printf("Not started:  %s! \n", ringInfo.ownId);
      break;
      case ELECTION:
      	ringInfo.participant = true;
        strncpy(message, election_str, strlen(election_str));
        strncat(message, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("ELECTION:  %s \n", message);
      break;
      case ELECTION_OVER:
        strncpy(message, election_over_str, strlen(election_over_str));
        strncat(message, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("ELECTION_OVER %s \n",message);
      break;
      case MESSAGE:
        printf("MESSAGE:  1%s \n", message);
        strncpy(message, message_str, strlen(message_str));
        printf("MESSAGE: 2 %s \n", message);
        printf("HIGHEST ID: 2 %s \n",ringInfo.highestId);
        strncat(message, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("MESSAGE: 3 %s \n", message);

        strncat(message, ringInfo.message, strlen(ringInfo.message));
        printf("MESSAGE: 4 %s \n", message);
        break;
      default:
        die("Something is wrong.. \n");
        break;
    }
    send(client_fd, message, sizeof(message), 0);
    printf("Going to sleep.... %d\n", count);
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
                    "Errno: %s", strerror(errno));
    sleep(2);
  }


  freeaddrinfo(result);

  return client_fd;

}
