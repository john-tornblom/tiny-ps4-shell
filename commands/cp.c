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
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "_common.h"


#define BUFSIZE 1024*8


static int
copy_file(const char* src, const char *dst) {
  int srcfd, dstfd;
  struct stat srcstat;
  uint8_t buf[BUFSIZE];
  int len;

  if(stat(src, &srcstat) != 0) {
    return -1;
  }

  if((srcfd = open(src, O_RDONLY)) < 0) {
    return -1;
  }

  int flags = O_CREAT | O_WRONLY | O_TRUNC;
  int mode = srcstat.st_mode;
  if((dstfd = open(dst, flags, mode)) < 0) {
    return -1;
  }

  int rc = 0;
  while((len = read(srcfd, buf, BUFSIZE)) > 0) {
    if(write(dstfd, buf, len) != len) {
      rc = -1;
      break;
    }
  }

  close(srcfd);
  close(dstfd);

  return 0;
}


/**
 *
 **/
int
main_cp(int argc, char **argv) {
  if(argc <= 1) {
    fprintf(stderr, "%s: missing operand\n", argv[0]);
    return -1;
  }

  char* dest = abspath(argv[argc-1]);
  
  for(int i=0; i<argc-2; i++) {
    char *src = abspath(argv[i+1]);
    
    if(copy_file(src, dest)) {
      perror("cp");
    }
    
    free(src);
  }

  free(dest);

  return 0;
}

