#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include "ringmain.c"


void setupServerConnection(){
  //Setup server connection


  struct addrinfo &hints = fillinAddrInfo(0, inputArg->remotePort);
  struct addrinfo* result=0;
  int err=getaddrinfo(hostname,portname,&hints,&results);
  if (err!=0) {
      die("failed to resolve remote socket address (err=%d)",err);
  }

  int server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (server_fd ==-1){
    die("Error when creating socet: %s", strerror(errno));
  }

  int resueaddr=1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(resueaddr), sizeof(&resueaddr)) == -1){
    die("Error while setting socket option: %s",strerror(errno));
  }

  if(bind(server_fd, result->ai_addr, result->ai_addrlen) == -1){
    die("Error while binding server socket: %s",strerror(errno));
  }
  freeaddrinfo(res);

  if (listen(server_fd, SOMAXCONN)){
    die("Error: Coult not listen for connection: %s", strerror(errno));
  }



}
