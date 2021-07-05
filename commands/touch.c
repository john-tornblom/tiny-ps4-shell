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
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>


/**
 *
 **/
int
main_touch(int argc, const char ** argv) {
  const char *name;
  int fd;
  struct utimbuf now;
  int r;

  r = 0;
  time(&now.actime);
  now.modtime = now.actime;

  while(argc-- > 1) {
    name = *(++argv);

    if((fd = open(name, O_CREAT | O_WRONLY | O_EXCL, 0666)) >= 0) {
      close(fd);
      continue;
    }

    if(errno != EEXIST) {
      perror(name);
      r = 1;
      continue;
    }

    if(errno != EEXIST) {
      perror(name);
      continue;
    }

    if(utime(name, &now) < 0) {
      perror(name);
      r = 1;
    }
  }

  return r;
}


