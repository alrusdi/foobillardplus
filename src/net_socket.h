/* net_socket.h
**
**    code for network-play
**
**    Version foobillard++ started at 12/2010
**    Copyright (C) 2010/2011 Holger Schaekel (foobillardplus@go4more.de)
**
**    good side for network functions:
**    http://beej.us/guide/bgnet/output/html/multipage/index.html
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

#ifndef NET_SOCKET_H
#define NET_SOCKET_H

TCPsocket host_create_socket(char *net_ip);
void net_close_listener(TCPsocket);
char* get_ip_address(void);
char* get_hostname(void);
int socket_read(TCPsocket socket, char *buffer, int nbytes);
int socket_write(TCPsocket socket, char *buffer, int nbytes);

#endif
