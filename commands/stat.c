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

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdint.h>

#include "_common.h"


int
main_stat(int argc, char **argv) {
  struct stat statbuf;
  
  if(argc <= 1) {
    fprintf(stderr, "%s: missing operand\n", argv[0]);
    return -1;
  }

  char *path = abspath(argv[1]);
    
  if(stat(path, &statbuf) != 0) {
    perror(path);
  } else {
    printf("filename: %s\n", argv[1]);
    printf("size: %lu\n", (unsigned long)statbuf.st_size);
    printf("blocks: %lu\n", (unsigned long)statbuf.st_blocks);
    printf("mode: %x\n", (unsigned)statbuf.st_mode);
    printf("uid: %u\n", (unsigned)statbuf.st_uid);
    printf("gid: %u\n", (unsigned)statbuf.st_gid);
    printf("dev: %lx\n", (unsigned long)statbuf.st_dev);
    printf("ino: %lu\n", (unsigned long)statbuf.st_ino);
    printf("nlink: %lu\n", (unsigned long)statbuf.st_nlink);
    printf("rdev: %lx\n", (unsigned long)statbuf.st_rdev);
    printf("atime: %lu\n", (unsigned long)statbuf.st_atime);
    printf("mtime: %lu\n", (unsigned long)statbuf.st_mtime);
    printf("ctime: %lu\n", (unsigned long)statbuf.st_ctime);
    printf("blksize: %lu\n", (unsigned long)statbuf.st_blksize);
  }

  free(path);
  
  return 0;
}



