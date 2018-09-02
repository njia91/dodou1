
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "server.h"

int listenForIncommingConnection(int server_fd){
  int connection_fd;
  printf("Listening for connections..\n");
  while((connection_fd = accept (server_fd, 0, 0)) == -1){
      if (errno == EAGAIN ){
        fprintf(stderr, "%s",strerror(errno));
      } else {
        die(strerror(errno));

      }
  }
  return connection_fd;
}

int setupServerConnection(const int localPort){
  int server_fd;
  struct addrinfo* result=0;

  fillinAddrInfo(&result, localPort, NULL, AI_PASSIVE);

  server_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (server_fd == -1){
    die(strerror(errno));
  }

  int resueaddr=1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(resueaddr), sizeof(&resueaddr)) == -1){
    close(server_fd);
    die(strerror(errno));
  }

  if(bind(server_fd, result->ai_addr, result->ai_addrlen) == -1){
    close(server_fd);
    die(strerror(errno));
  }
  freeaddrinfo(result);

  if (listen(server_fd, SOMAXCONN)){
    die(strerror(errno));
  }
  return server_fd;
}

void verifyCurrentRingPhase(char *buffer){
  for (int i = 0; i < strlen(buffer) && buffer[i] != '\0'; i++){
    if(buffer[i] == '\n'){
      if(!(strncmp(buffer, ELECTION_STR, i))){
        ringInfo.currentPhase = ELECTION;
      }else if(!(strncmp(buffer, ELECTION_OVER_STR, i))){
        ringInfo.currentPhase = ELECTION_OVER;
      }else if(!(strncmp(buffer, MESSAGE_STR, i))){
        ringInfo.currentPhase = MESSAGE;
      } else{
        fprintf(stderr, "Invalid Message format - Read the instructions...\n");
        ringInfo.ringActive = false;
      }
      memset(ringInfo.receivedMessage, '\0', sizeof(ringInfo.receivedMessage));
      strncpy(ringInfo.receivedMessage, &buffer[i + 1], strlen(&buffer[i + 1]));
      break;
    }
  }
}

void handleConnectionSession(int connection_fd){
  char buffer[100];
  bool active = true;
  int ret;
  int messageCount = 0;
  while (active){
    memset(buffer, '\0', PACKET_SIZE);
    ret = recv(connection_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
    pthread_mutex_lock(&mtxRingInfo);

    if (ret == 0){
      fprintf(stderr, "Socket peer has performed a shutdown."
                      "terminating ring \n");
      ringInfo.ringActive = false;
    } else if (ret < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)){
      fprintf(stderr," recv return error value: %s", strerror(errno));
      ringInfo.ringActive = false;
    } else if(ret > 0){
      printf("\nReceived message No. %d:\n%s", messageCount, buffer);
      verifyCurrentRingPhase(buffer);
      // Notifes client of incoming message.
  		pthread_cond_broadcast(&newMessage);
      messageCount++;
    }
    if(!ringInfo.ringActive){
      pthread_cond_broadcast(&newMessage);
      active = false;
    }
    pthread_mutex_unlock(&mtxRingInfo);
  }

}

void serverMain(int localPort){
  int server_fd = setupServerConnection(localPort);
  int connection_fd = listenForIncommingConnection(server_fd);
  handleConnectionSession(connection_fd);
  close(server_fd);
  close(connection_fd);
}
