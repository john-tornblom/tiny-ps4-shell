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

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

/**
 * 
 **/
int
main_kill(int argc, char **argv) {
  pid_t pid;
  int sig = SIGTERM;
  int c;
  
while ((c = getopt(argc, argv, "s:")) != -1) {
    switch (c) {
    case 's':
      sig = atoi(optarg);
      break;
    }
  }

 if(optind >= argc || !isdigit(argv[optind][0])) {
   printf("usage: %s [-s signum] <pid>\n", argv[0]);
   return EXIT_FAILURE;
 }

  pid = atoi(argv[optind]);
  
  if(kill(pid, sig)) {
    perror(argv[0]);
    return -1;
  }
    
  return 0;
}


