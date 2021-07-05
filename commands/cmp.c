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

// Code inspired by http://members.tip.net.au/%7Edbell/programs/sash-3.8.tar.gz

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

#define BUF_SIZE 8192


/**
 * state variable for catching user interpt signals
 **/
static int interupted = 0;


/**
 * Catch user interupts
 **/
static void
on_SIGINT(int val) {
  interupted = 1;
}


/**
 *
 **/
int
main_cmp(int argc, const char ** argv) {
  int fd1;
  int fd2;
  int cc1;
  int cc2;
  long pos;
  const char *bp1;
  const char *bp2;
  char buf1[BUF_SIZE];
  char buf2[BUF_SIZE];
  struct stat statBuf1;
  struct stat statBuf2;
  int r;

  r = 0;

  signal(SIGINT, on_SIGINT);
  
  if(stat(argv[1], &statBuf1) < 0){
    perror(argv[1]);
    return 1;
  }

  if(stat(argv[2], &statBuf2) < 0) {
    perror(argv[2]);
    return 1;
  }

  if((statBuf1.st_dev == statBuf2.st_dev) &&
      (statBuf1.st_ino == statBuf2.st_ino)) {
    printf("Files are links to each other\n");
    return 0;
  }

  if(statBuf1.st_size != statBuf2.st_size) {
    printf("Files are different sizes\n");
    return 1;
  }

  fd1 = open(argv[1], O_RDONLY);

  if (fd1 < 0) {
    perror(argv[1]);
    return 1;
  }

  if((fd2 = open(argv[2], O_RDONLY)) < 0) {
    perror(argv[2]);
    close(fd1);
    return 1;
  }

  pos = 0;
  while(1) {
    if(interupted) {
      goto closefiles;
    }
    
    if((cc1 = read(fd1, buf1, sizeof(buf1))) < 0) {
      perror(argv[1]);
      r = 1;
      goto closefiles;
    }

    if((cc2 = read(fd2, buf2, sizeof(buf2))) < 0) {
      perror(argv[2]);
      r = 1;
      goto closefiles;
    }

    if ((cc1 == 0) && (cc2 == 0)) {
      printf("Files are identical\n");
      r = 0;
      goto closefiles;
    }

    if(cc1 < cc2) {
      printf("First file is shorter than second\n");
      r = 1;
      goto closefiles;
    }

    if(cc1 > cc2) {
      printf("Second file is shorter than first\n");
      r = 1;
      goto closefiles;
    }

    if(memcmp(buf1, buf2, cc1) == 0) {
      pos += cc1;
      continue;
    }

    bp1 = buf1;
    bp2 = buf2;

    while(*bp1++ == *bp2++) {
      pos++;
    }
    printf("Files differ at byte position %ld\n", pos);
    r = 1;

    goto closefiles;
  }

 closefiles:
  close(fd1);
  close(fd2);
  
  return r;
}

