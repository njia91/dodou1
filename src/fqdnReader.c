#define _DEFAULT_SOURCE 1

#include <unistd.h>
#include <netdb.h>
#include <string.h>
int getFQDN(char *fqdn, size_t n){
  char hostname[256];
  int r = gethostname(hostname, 256);

  if(r != 0){
    return 1;
  }
  struct addrinfo h = {0};
  h.ai_family = AF_INET;
  h.ai_socktype = SOCK_STREAM;
  h.ai_flags = AI_CANONNAME;

  struct addrinfo *info;
  if(getaddrinfo(hostname, NULL, &h, &info) != 0){
    return 2;
  }

  strncpy(fqdn, info->ai_canonname, n);
  freeaddrinfo(info);

  return 0;
}
