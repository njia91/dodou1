#include "client.h"
#include <stdbool.h>

void clientMain(const char *remoteAdress, const int remotePort){
  int client_fd = setupConnectionToServer(remoteAdress, remotePort);
  sendMessagesToServer(client_fd);
}

void sendMessagesToServer(int client_fd){
  bool active = true;
  char message[100] = "Sending message....";
  while(active){
    printf("Sending message!!! %lu \n", pthread_self());
    send(client_fd, message, sizeof(message), 0);
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

  int err = getaddrinfo("localhost", portId,&hints,&result);
  if (err!=0) {
      die(gai_strerror(err));
  }

  client_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (client_fd ==-1){
    die(strerror(errno));
  }

  while(connect(client_fd, result->ai_addr, result->ai_addrlen) != -1){
    fprintf(stderr, "%s", strerror(errno));
    sleep(2);
  }


  freeaddrinfo(result);

  return client_fd;

}
