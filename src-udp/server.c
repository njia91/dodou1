
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

void handleConnectionSession(int connection_fd){
  struct sockaddr_in senderAddr;
  socklen_t senderAddrLen = sizeof(senderAddr);
  clock_t totalTime = 0;
  char buffer[100];
  bool active = true;
  int ret;
  int lapCount = 0;
  while (active){
    memset(buffer, '\0', PACKET_SIZE);
    memset(&senderAddr, 0, senderAddrLen);
    ret = recvfrom(connection_fd, buffer, PACKET_SIZE, 0,
                  (struct sockaddr*) &senderAddr, &senderAddrLen);
    pthread_mutex_lock(&mtxRingInfo);

    // Check if package is smaller than what the protocol stated.
    if (ret < PACKET_SIZE && ret > 0){
      fprintf(stderr, "read() returned %d. Should have returned %d \n", ret, PACKET_SIZE);
    }
    fprintf(stderr," SERVER TAR EMOT MEDDELANDEN \n");

    if (ret == 0){
      fprintf(stderr, "Socket peer has performed a shuwdown."
                      "terminating ring \n");
      ringInfo.ringActive = false;
    } else if (ret < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)){
      fprintf(stderr," recv return error value: %s", strerror(errno));
      ringInfo.ringActive = false;
    } else if (ret > 0){
      lapCount++;
      getPacketInformation(buffer);
      printf("\nReceived message No. %d:\n%s", lapCount, buffer);
      if (ringInfo.ringLeader == true){
        totalTime = clock() - ringInfo.startTime;
        printf("Average lap time: %lu (ms)\n", (long int) totalTime / lapCount);
      }
    }

    if(!ringInfo.ringActive){
      active = false;
    }

    // Notify client of incoming message.
    pthread_cond_broadcast(&newMessage);
    pthread_mutex_unlock(&mtxRingInfo);

    sleep(2);
  }
  fprintf(stderr, "Terminating Server \n");
}

void serverMain(int localPort){
  int server_fd = setupUdpServerSocket(localPort);
  handleConnectionSession(server_fd);
  close(server_fd);
}
