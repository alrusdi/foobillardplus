/*
 * ip1.c
 *
 *  Created on: 09.07.2011
 *      Author: holger
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>


void show_address_info( struct ifaddrs *ifa ){
  struct sockaddr_in *s4;
  struct sockaddr_in6 *s6;
  /* ipv6 addresses have to fit in this buffer */
  char buf[64];

  if (AF_INET == ifa->ifa_addr->sa_family){
    s4 = (struct sockaddr_in *)(ifa->ifa_addr);
    if (NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf))){
      printf("%s: inet_ntop failed!\n", ifa->ifa_name);
    } else {
      printf("IPv4 addr %s: %s\n", ifa->ifa_name, buf);
    }
  }
  else if (AF_INET6 == ifa->ifa_addr->sa_family) {
    s6 = (struct sockaddr_in6 *)(ifa->ifa_addr);
    if (NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s6->sin6_addr), buf, sizeof(buf))) {
      printf("%s: inet_ntop failed!\n", ifa->ifa_name);
    } else {
      printf("IPv6 addr %s: %s\n", ifa->ifa_name, buf);
      }
  }

}


int main(int argc, char **argv){
  struct ifaddrs *myaddrs, *ifa;
  int status;

  status = getifaddrs(&myaddrs);
  if (status != 0){
    perror("getifaddrs failed!");
    exit(1);
  }

  for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next){
    if (NULL == ifa->ifa_addr){
      continue;
    }
    if ((ifa->ifa_flags & IFF_UP) == 0) {
      continue;
    }
    show_address_info(ifa);
  }
  freeifaddrs(myaddrs);
  return 0;
}
