
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "server.h"

int setupUdpServerSocket(const int localPort){
  int server_fd;
  struct addrinfo* result=0;

  fillinAddrInfo(&result, localPort, NULL, AI_PASSIVE);

  server_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (server_fd == -1){
    die(strerror(errno));
  }

  if(bind(server_fd, result->ai_addr, result->ai_addrlen) == -1){
    close(server_fd);
    die(strerror(errno));
  }
  freeaddrinfo(result);

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
        fprintf(stderr, "Invalid Message formate - Read the instructions...\n");
        ringInfo.ringActive = false;
      }
      memset(ringInfo.receivedMessage, '\0', sizeof(ringInfo.receivedMessage));
      strncpy(ringInfo.receivedMessage, &buffer[i + 1], strlen(&buffer[i + 1]));
      break;
    }
  }
}

void handleConnectionSession(int connection_fd){
  struct sockaddr_in senderAddr;
  socklen_t senderAddrLen = sizeof(senderAddr);
  char buffer[100];
  bool active = true;
  int ret;
  int messageCount = 0;
  while (active){
    memset(buffer, '\0', PACKET_SIZE);
    memset(&senderAddr, 0, senderAddrLen);
    ret = recvfrom(connection_fd, buffer, sizeof(buffer), 0,
                  (struct sockaddr*) &senderAddr, &senderAddrLen);
    pthread_mutex_lock(&mtxRingInfo);

    if (ret == 0){
      fprintf(stderr, "Socket peer has performed a shuwdown."
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

    if(ringInfo.currentPhase == NOT_STARTED || ringInfo.currentPhase == ELECTION){
      pthread_cond_broadcast(&newMessage);
    }

    if(!ringInfo.ringActive){
      pthread_cond_broadcast(&newMessage);
      active = false;
    }
    pthread_mutex_unlock(&mtxRingInfo);
  }

}

void serverMain(int localPort){
  int server_fd = setupUdpServerSocket(localPort);
  handleConnectionSession(server_fd);
  close(server_fd);
}
