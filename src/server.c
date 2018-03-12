
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

void prepareMessage(){
    switch(ringInfo.currentPhase){
      case NOT_STARTED:
        strncpy(ringInfo.packetToSend, election_str, strlen(election_str));
        strncat(ringInfo.message, ringInfo.ownId, strlen(ringInfo.ownId));
        printf("Not started:  %s! \n", ringInfo.ownId);
      break;
      case ELECTION:
      	ringInfo.participant = true;
        strncpy(ringInfo.packetToSend, election_str, strlen(election_str));
        strncat(ringInfo.packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("ELECTION:  %s \n", ringInfo.packetToSend);
      break;
      case ELECTION_OVER:
        strncpy(ringInfo.packetToSend, election_over_str, strlen(election_over_str));
        strncat(ringInfo.packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
        printf("ELECTION_OVER %s \n",ringInfo.packetToSend);
      	break;
      case MESSAGE:
        break;
      default:
        die("Something is wrong.. \n");
        break;
    }		

}

void handleConnectionSession(int connection_fd){
  char buffer[100];
  bool ringActive = true;
  int ret;
  char *recievedMessage;
	bool shouldMessageBeForwarded = false;
  while (ringActive){
    memset(buffer, 0, sizeof(buffer));

    printf("Handle connection.....!!!! %lu \n", pthread_self());
    ret = recv(connection_fd, buffer, sizeof(buffer), 0);
    if (ret == -1){
      die(strerror(errno));
    }
    printf("Message from Client %s \n", buffer);
    pthread_mutex_lock(&mtxRingInfo);
    for (int i = 0; i < strlen(buffer) && buffer[i] != '\0'; i++){
      if(buffer[i] == '\n'){
        if(!(strncmp(buffer, election_str, i))){
          ringInfo.currentPhase = ELECTION;
        }else if(!(strncmp(buffer, election_over_str, i))){
          ringInfo.currentPhase = ELECTION_OVER;
        }else if(!(strncmp(buffer, message_str, i))){
          ringInfo.currentPhase = MESSAGE;
        } else{
          die("Invalid Message formate - Read the instructions...");
        }
        recievedMessage = &buffer[i + 1];
        break;
      }
    }


    printf("SERVER ELECTION:  %d \n", ringInfo.currentPhase);
    printf("SERVER MESSAGE:  %s \n", recievedMessage);
    switch(ringInfo.currentPhase){
      case ELECTION:
        //Compare NODE ID
        ret = strcmp(recievedMessage, ringInfo.highestId);
        if(ret > 0 ){
          printf("ID BIFFER THEN OTHER...!!!%s \n", recievedMessage);
					shouldMessageBeForwarded = true;
          ringInfo.highestId = recievedMessage;
        }
			  else if (ret < 0 && ringInfo.participant == true){
					shouldMessageBeForwarded = false;
				}	
				else if ( ret < 0 && ringInfo.participant == false){
					
					shouldMessageBeForwarded = true;				
				}	
				else if ( ret == 0){
          printf("message phase entered!!!%s \n", recievedMessage);
          ringInfo.currentPhase = ELECTION_OVER;
          ringInfo.participant = false;
        }
      	break;
      case ELECTION_OVER:
      //DO NOTHING?
      	if ( strcmp(recievedMessage, ringInfo.ownId) == 0){
	      	ringInfo.currentPhase = MESSAGE;
      	}
      //  printf("SERVER ELECTION_OVER %s %lu \n",message);
      	break;
      case MESSAGE:
      //SAVE MESSAGE?
    //  printf("SERVER MESSAGE:  %s \n", message);
      case NOT_STARTED:
      default:
      //  die("Something is wrong.. \n");
        break;
    }
		if(shouldMessageBeForwarded){
			prepareMessage();
			pthread_cond_broadcast(&newMessage);
		}		
    pthread_mutex_unlock(&mtxRingInfo);
  }
}

void *serverMain(void *localPort){
  int portId =  *(int *)localPort;
  int server_fd = setupServerConnection(portId);
  int connection_fd = listenForIncommingConnection(server_fd);
  handleConnectionSession(connection_fd);

  pthread_exit(NULL);
}
