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
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pwd.h>
#include <unistd.h>

/**
 *
 **/
int
main_chown(int argc, const char ** argv) {
  const char *cp;
  int uid;
  struct passwd *pwd;
  struct stat statBuf;
  int r;
  
  r = 0;
  cp = argv[1];

  if(isdigit(*cp)) {
    uid = 0;

    while(isdigit(*cp))
      uid = uid * 10 + (*cp++ - '0');

    if(*cp) {
      fprintf(stderr, "Bad uid value\n");
      return 1;
    }
  } else {
    if(!(pwd = getpwnam(cp))) {
      fprintf(stderr, "Unknown user name\n");
      return 1;
    }

    uid = pwd->pw_uid;
  }

  argc--;
  argv++;

  while(argc-- > 1) {
    argv++;

    if((stat(*argv, &statBuf) < 0) ||
       (chown(*argv, uid, statBuf.st_gid) < 0)) {
      perror(*argv);
      r = 1;
    }
  }

  return r;
}

