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
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>


/**
 *
 **/
int
main_chgrp(int argc, const char ** argv) {
  const char *cp;
  int gid;
  struct group *grp;
  struct stat statBuf;
  int r;

  r = 0;
  cp = argv[1];
  
  if(isdigit(*cp)) {
    gid = 0;
      
    while(isdigit(*cp))
      gid = gid * 10 + (*cp++ - '0');

    if (*cp) {
      fprintf(stderr, "Bad gid value\n");  
      return 1;
    }
  } else {
    grp = getgrnam(cp);
    
    if (grp == NULL) {
      fprintf(stderr, "Unknown group name\n");
      return 1;
    }

    gid = grp->gr_gid;
  }

  argc--;
  argv++;

  while (argc-- > 1) {
    argv++;

    if ((stat(*argv, &statBuf) < 0) ||
	(chown(*argv, statBuf.st_uid, gid) < 0)) {
      perror(*argv);
      r = 1;
    }
  }
  
  return r;
}
