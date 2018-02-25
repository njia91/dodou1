
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#include "server.h"

int listenForIncommingConnection(int server_fd){
  int connection_fd;
  while((connection_fd = accept (server_fd, 0, 0)) == -1){

      if (errno == EAGAIN ){
        printf("Can not find stuf........!!!! %lu \n", pthread_self());
        fprintf(stderr, "%s",strerror(errno));
      } else {
        printf("Die Listening.........!!!! %lu \n", pthread_self());
        die(strerror(errno));

      }
  }
  printf("Finish listening....!!!! %lu \n", pthread_self());
  return connection_fd;

}

int setupServerConnection(const int localPort){
  //Setup server connection
  int server_fd;
  char portId[15];
  sprintf(portId, "%d", localPort);
  struct addrinfo hints;
  memset(&hints,0,sizeof(hints));
  fillinAddrInfo("0", localPort, &hints);
  struct addrinfo* result=0;
  int err = getaddrinfo("localhost",portId,&hints,&result);
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
  bool ringActive = true;
  int ret;
  int phase;
  char *message;

  while (ringActive){
    memset(buffer, 0, sizeof(buffer));

    printf("Handle connection.....!!!! %lu \n", pthread_self());
    ret = recv(connection_fd, buffer, sizeof(buffer), 0);
    if (ret == -1){
      die(strerror(errno));
    }
    printf("Message from Client %s \n", buffer);
    for (int i = 0; i < strlen(buffer) && buffer[i] != '\0'; i++){
      if(buffer[i] == '\n'){
        if(strncmp(buffer, "ELECTION", i)){
          phase = ELECTION;
        }else if(strncmp(buffer, "ELECTION_OVER", i)){
          phase = ELECTION_OVER;
        }else if(strncmp(buffer, "MESSAGE", i)){
          phase = ELECTION_OVER;
        } else{
          die("Invalid Message formate - Read the instructions...");
        }
        printf("SERVER ELECTION:  %d \n", phase);
        message = &buffer[i + 1];
        break;
      }

    }
    printf("SERVER ELECTION:  %d \n", phase);
    printf("SERVER ELECTION:  %s \n", message);
    // switch(ringInfo.currentPhase){
    //   case ELECTION:
    //     pthread_mutex_lock(&mtxRingInfo);
    //     //printf("SERVER ELECTION:  %s \n", message);
    //   break;
    //   case ELECTION_OVER:
    //
    //   //  printf("SERVER ELECTION_OVER %s %lu \n",message);
    //   break;
    //   case MESSAGE:
    //
    // //  printf("SERVER MESSAGE:  %s \n", message);
    //   default:
    //   //  die("Something is wrong.. \n");
    //     break;
    // }


  }
}

void *serverMain(void *localPort){
  int portId =  *(int *)localPort;
  int server_fd = setupServerConnection(portId);
  int connection_fd = listenForIncommingConnection(server_fd);
  handleConnectionSession(connection_fd);

  pthread_exit(NULL);
}
