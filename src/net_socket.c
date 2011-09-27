/* net_socket.c
**
**    code for network-play
**
**    Version foobillard++ started at 12/2010
**    Copyright (C) 2010/2011 Holger Schaekel (foobillardplus@go4more.de)
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License Version 2 as
**    published by the Free Software Foundation;
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/

#ifdef NETWORKING
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include <SDL.h>
#include <SDL_net.h>

#ifdef __MINGW32__ //RB
   #include <winsock2.h>
#else
	  #include <sys/socket.h>
  	#include <arpa/inet.h>
  	#include <net/if.h>
  	#include <ifaddrs.h>
  	#include <netinet/in.h>
#endif
#include "options.h"
#include "sys_stuff.h"
#include "language.h"

   static IPaddress addr;
   static TCPsocket sockethandle;

/***********************************************************************
 *     Create a tcp-socket and return a socket-handle for it           *
 *     NULL on error                                                   *
 *     Server / Client                                                 *
 ***********************************************************************/

TCPsocket host_create_socket(char *net_ip)
{
   if (SDLNet_Init() < 0) {
     fprintf (stderr,"ERR SDL-Net: %s\n", SDLNet_GetError ());
     sockethandle = NULL;
     return (NULL);
   }
   if (SDLNet_ResolveHost (&addr, net_ip, options_net_portnum) < 0) {
     fprintf (stderr,"ERR ResolveHost: %s\n", SDLNet_GetError ());
     sockethandle = NULL;
     SDLNet_Quit();
     return (NULL);
   }
   sockethandle = SDLNet_TCP_Open (&addr);
   if (sockethandle == NULL) {
     fprintf (stderr,"ERR TCP_Open: %s\n", SDLNet_GetError ());
     SDLNet_Quit();
     return (NULL);
   }
   return (sockethandle);
}

/***********************************************************************
 *                     close a net listener                            *
 ***********************************************************************/

void net_close_listener(TCPsocket socket)
{
  if(socket==NULL) {
     return;
  }
  SDLNet_TCP_Close(socket);
}

#ifndef __MINGW32__	// RB

/***********************************************************************
 *     resolve one ip-address of the host                              *
 *     only internal use in net_socket.c                               *
 *     NULL on error or false or not usable adresses                   *
 *     Server / Client                                                 *
 ***********************************************************************/

char * show_address_info( struct ifaddrs *ifa ){
  struct sockaddr_in *s4;
  struct sockaddr_in6 *s6;
  /* ipv6 addresses have to fit in this buffer */
  char buf[64];
  static char ipanswer[80];

  if (AF_INET == ifa->ifa_addr->sa_family){
    s4 = (struct sockaddr_in *)(ifa->ifa_addr);
    if (NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf))){
      fprintf(stderr,"%s: inet_ntop failed!\n", ifa->ifa_name);
      return(NULL);
    } else {
      if(!strncmp(buf,"127.0",5) || !strncmp(ifa->ifa_name,"lo",2)) {
        return(NULL);
      }
      sprintf(ipanswer,"IPv4: %s",buf);
      return(ipanswer);
    }
  }
  else if (AF_INET6 == ifa->ifa_addr->sa_family) {
    s6 = (struct sockaddr_in6 *)(ifa->ifa_addr);
    if (NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s6->sin6_addr), buf, sizeof(buf))) {
      fprintf(stderr,"%s: inet_ntop failed!\n", ifa->ifa_name);
      return(NULL);
    } else {
      if(buf[0] == ':' || !strncmp(ifa->ifa_name,"lo",2)) {
        return(NULL);
      }
      sprintf(ipanswer,"IPv6: %s",buf);
      return(ipanswer);
    }
  }
return(NULL);
}

/***********************************************************************
 *     resolve local ip-addresses                                      *
 *     0 on error                                                      *
 *     Server / Client                                                 *
 ***********************************************************************/

char* get_ip_address(void)
{

 struct ifaddrs *myaddrs, *ifa;
 int status;
 char *ipstring;
 static char ip_adresses[4096];

 ip_adresses[0] = 0;
 status = getifaddrs(&myaddrs);
 if (status != 0){
   perror("getifaddrs failed!");
   return(NULL);
 }

 for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next){
   if (NULL == ifa->ifa_addr){
     continue;
   }
   if ((ifa->ifa_flags & IFF_UP) == 0) {
     continue;
   }
   if((ipstring = show_address_info(ifa))!=NULL) {
     //fprintf(stderr,"%s\n",show_address_info(ifa));
     strcat(ip_adresses,ipstring);
     strcat(ip_adresses,";");
   }
 }
 freeifaddrs(myaddrs);

   return(ip_adresses);
}

#endif

/***********************************************************************
 *     resolve the local hostname                                      *
 *     Noname on error                                                 *
 *     Server / Client                                                 *
 ***********************************************************************/

char* get_hostname(void)
{
   static char szHostName[255];
   static char *host;
   host = localeText[225];
   if(!gethostname(szHostName, 255)) {
      host = szHostName;
   }
   //fprintf(stderr,"Host: %s\n",host);
   return (host);
}

/***********************************************************************
 *                 Read data from a connected socket                   *
 *         Returns 0 on error, or count of bytes read                  *
 ***********************************************************************/

int socket_read(TCPsocket socket, char *buffer, int nbytes)
{
  int count;
  if(socket <0 || !buffer || !nbytes) {
     return(0);
  }
  count = SDLNet_TCP_Recv (socket, buffer, nbytes);
  return(count);
}

/***********************************************************************
 *                  Send data to a connected socket                    *
 *         Returns 0 on error, or count of bytes send                  *
 ***********************************************************************/

int socket_write(TCPsocket socket, char *buffer, int nbytes)
{
  int count;
  if(socket <0 || !buffer || !nbytes) {
     return(0);
  }
  count = SDLNet_TCP_Send (socket, buffer, nbytes);
  return(count);
}
#endif
