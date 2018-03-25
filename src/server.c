
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "server.h"

int listenForIncommingConnection(int server_fd){
  int connection_fd;
  while((connection_fd = accept (server_fd, 0, 0)) == -1){
      if (errno == EAGAIN ){
        fprintf(stderr, "%s",strerror(errno));
      } else {
        die(strerror(errno));

      }
  }
  return connection_fd;
}

int setupServerConnection(const int localPort, const char* remoteIP){
  //Setup server connection
  int server_fd;
  char portId[15];
  sprintf(portId, "%d", localPort);
  struct addrinfo hints;
  memset(&hints,0,sizeof(hints));
  fillinAddrInfo("0", localPort, &hints);
  struct addrinfo* result=0;
  int err = getaddrinfo(remoteIP, portId,&hints,&result);
  if (err!=0) {
      die(gai_strerror(err));
  }

  server_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (server_fd ==-1){
    die(strerror(errno));
  }

  int resueaddr=1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(resueaddr), sizeof(&resueaddr)) == -1){
    die(strerror(errno));
  }

  if(bind(server_fd, result->ai_addr, result->ai_addrlen) == -1){
    die(strerror(errno));
  }

  freeaddrinfo(result);

  if (listen(server_fd, SOMAXCONN)){
    die(strerror(errno));
  }
  return server_fd;
}

void handleConnectionSession(int connection_fd){
  char buffer[100];
  int ret;

  while (ringInfo.ringActive){
    memset(buffer, 0, PACKET_SIZE);

    ret = recv(connection_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
    pthread_mutex_lock(&mtxRingInfo);
    if (ret < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)){
      fprintf(stderr," recv return error value: %s", strerror(errno));
      ringInfo.ringActive = false;
    } else if(ret >= 0){
      printf("Received message:: %s", buffer);
      for (int i = 0; i < strlen(buffer) && buffer[i] != '\0'; i++){
        if(buffer[i] == '\n'){
          if(!(strncmp(buffer, ELECTION_STR, i))){
            ringInfo.currentPhase = ELECTION;
          }else if(!(strncmp(buffer, ELECTION_OVER_STR, i))){
            ringInfo.currentPhase = ELECTION_OVER;
          }else if(!(strncmp(buffer, MESSAGE_STR, i))){
            ringInfo.currentPhase = MESSAGE;
          } else{
            fprintf(stderr, "Invalid Message formate - Read the instructions...\n");
						ringInfo.ringActive = false;
          }
          strncpy(ringInfo.receivedMessage, &buffer[i + 1], strlen(&buffer[i + 1]));
          break;
        }
      }
  		pthread_cond_broadcast(&newMessage);
    }
    pthread_mutex_unlock(&mtxRingInfo);
  }
}

void *serverMain(void *inputArg){
	nodeArg arg = *(nodeArg *) inputArg;
  int server_fd = setupServerConnection( arg.remotePort, arg.remoteIP);
  int connection_fd = listenForIncommingConnection(server_fd);
  handleConnectionSession(connection_fd);

  pthread_exit(NULL);
}
