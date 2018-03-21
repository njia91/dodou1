#include "client.h"


void clientMain(const char *remoteAdress, const int remotePort){
  int client_fd = setupConnectionToServer(remoteAdress, remotePort);
  sendMessagesToServer(client_fd);
}

void sendMessagesToServer(int client_fd){
  bool active = true;
  int count = 0;
  // if(ringInfo.currentPhase == NOT_STARTED){
  //   sleep(1);
  // }

  while(active){
        count++;
	  printf("BEGINING OF LOOP\n");
    pthread_mutex_lock(&mtxRingInfo);

    send(client_fd, ringInfo.message, sizeof(ringInfo.message), 0);
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
