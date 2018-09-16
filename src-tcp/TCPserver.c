
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "TCPserver.h"

int listenForIncommingConnection(int server_fd){
  int connection_fd;
  printf("Listening for connections..\n");
  // Listen for ONE incoming connection.
  while((connection_fd = accept(server_fd, 0, 0)) == -1){
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

  // Allow reuse of local addresses.
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

void handleConnectionSession(int connection_fd){
  char buffer[100];
  bool active = true;
  clock_t totalTime = 0;
  int ret;
  int lapCount = 0;
  while (active){
    memset(buffer, '\0', PACKET_SIZE);
    ret = recv(connection_fd, buffer, PACKET_SIZE, MSG_DONTWAIT);
    pthread_mutex_lock(&mtxRingInfo);

    // Check if package is smaller than what the protocol stated.
    if (ret < PACKET_SIZE && ret > 0){
      fprintf(stderr, "read() returned %d. Should have returned %d \n", ret, PACKET_SIZE);
    }

    // Socket peer has shutdown the connection.
    if (ret == 0){
      fprintf(stderr, "Socket peer has performed a shutdown."
                      "terminating ring \n");
      ringInfo.ringActive = false;
    } else if (ret < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)){
      fprintf(stderr," recv return error value: %s", strerror(errno));
      ringInfo.ringActive = false;
    } else if(ret > 0){
      getPacketInformation(buffer);
      lapCount++;
      printf("\nReceived message No. %d:\n %s", lapCount, buffer);
      if (ringInfo.ringLeader == true){
        totalTime = clock() - ringInfo.startTime;
        printf("Average lap time: %lu (ms)\n", (long int) totalTime / lapCount);
      }

      pthread_cond_broadcast(&newMessage);
    }

    // If error or shutdown is initiated - Notify client and terminate program.
    if(!ringInfo.ringActive){
      active = false;
      pthread_cond_broadcast(&newMessage);
    }

    pthread_mutex_unlock(&mtxRingInfo);
  }
  fprintf(stderr, "Terminating Server \n");
}

void serverMain(int localPort){
  int server_fd = setupServerConnection(localPort);
  int connection_fd = listenForIncommingConnection(server_fd);
  handleConnectionSession(connection_fd);
  close(server_fd);
  close(connection_fd);
}
