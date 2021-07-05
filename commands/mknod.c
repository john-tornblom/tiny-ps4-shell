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
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/**
 *
 **/
int
main_mknod(int argc, const char ** argv) {
  const char *cp;
  int mode;
  int major;
  int minor;

  mode = 0666;

  if(strcmp(argv[2], "b") == 0) {
    mode |= S_IFBLK;
  } else if (strcmp(argv[2], "c") == 0) {
    mode |= S_IFCHR;
  } else {
    fprintf(stderr, "Bad device type\n");
    return 1;
  }

  major = 0;
  cp = argv[3];

  while(isdigit(*cp)) {
    major = major * 10 + *cp++ - '0';
  }
  
  if (*cp || (major < 0) || (major > 255)) {
    fprintf(stderr, "Bad major number\n");
    return 1;
  }

  minor = 0;
  cp = argv[4];

  while(isdigit(*cp)) {
    minor = minor * 10 + *cp++ - '0';
  }
  
  if(*cp || (minor < 0) || (minor > 255)) {
    fprintf(stderr, "Bad minor number\n");
    return 1;
  }

  if(mknod(argv[1], mode, major * 256 + minor) < 0) {
    perror(argv[1]);
    return 1;
  }

  return 0;
}


