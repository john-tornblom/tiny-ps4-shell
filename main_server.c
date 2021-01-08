/* Copyright (C) 2021 John Törnblom

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <signal.h>

#include "sys.h"
#include "shell.h"


/**
 * open orbis has a couple of incorrect defines...
 **/
#ifdef __ORBIS__
#undef SO_REUSEADDR
#define SO_REUSEADDR 0x0004
#endif


/**
 * Close all inherited file descriptors, and remap stdio to the socket.
 **/
static int
client_init(int fd) {
  for(int i=0; i<1024; i++) {
    if(i != fd) {
      close(i);
    }
  }
  
  if(sys_dup2(fd, STDIN_FILENO) < 0) {
    return -1;
  }
    
  if(sys_dup2(fd, STDOUT_FILENO) < 0) {
    return -1;
  }
  
  if(sys_dup2(fd, STDERR_FILENO) < 0) {
    return -1;
  }

  close(fd);
  return 0;
}


/**
 * Entry point for new telnet connections.
 **/
static void
client_fork(int master, int slave) {
  pid_t pid = sys_fork();
  
  if (pid == 0) {
    close(master);
    if(client_init(slave)) {
      sys_notify("init: %s", strerror(errno));
    } else {
      shell_loop();
    }
    exit(0);
  } else if (pid < 0) {
    sys_notify("fork: %s", strerror(errno));
  }

  close(slave);
}


/**
 * Launch a simple telnet server on a given port.
 **/
void serve_telnet(unsigned short port) {
  int sockfd;
  int connfd;
  socklen_t addr_len;

  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    sys_notify("socket: %s", strerror(errno));
    return;
  }

  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
    sys_notify("setsockopt: %s", strerror(errno));
    return;
  }
  
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
    sys_notify("bind: %s", strerror(errno));
    return;
  }

  if(listen(sockfd, 5) != 0) {
    sys_notify("listen: %s", strerror(errno));
    return;
  }

  addr_len = sizeof(client_addr);
  while(1) {
    if((connfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len)) < 0) {
      sys_notify("accept: %s", strerror(errno));
      return;
    }

    client_fork(sockfd, connfd);
  }
}


/**
 * Application entry point.
 **/
int
main(void) {
  const int port = 2323;
  sys_notify("Serving telnet on port %d", port);

  sys_init();
  serve_telnet(port);
  
  while(1) {
    sleep(1);
  }

  return 0;
}

