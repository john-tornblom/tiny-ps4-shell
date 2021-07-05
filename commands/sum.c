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
#include <fcntl.h>
#include <unistd.h>


#define BUF_SIZE 8192


/**
 *
 **/
int
main_sum(int argc, const char ** argv) {
  const char *name;
  int fd;
  int cc;
  int ch;
  int i;
  unsigned long checksum;
  char buf[BUF_SIZE];
  int r;

  argc--;
  argv++;
  r = 0;

  while(argc-- > 0) {
    name = *argv++;
    
    if ((fd = open(name, O_RDONLY)) < 0) {
      perror(name);
      r = 1;
      continue;
    }

    checksum = 0;
    while ((cc = read(fd, buf, sizeof(buf))) > 0) {
      for (i = 0; i < cc; i++) {
	ch = buf[i];

	if ((checksum & 0x01) != 0) {
	  checksum = (checksum >> 1) + 0x8000;
	} else {
	  checksum = (checksum >> 1);
	}

	checksum = (checksum + ch) & 0xffff;
      }
    }

    if(cc < 0) {
      perror(name);
      r = 1;
      close(fd);
      continue;
    }

    close(fd);
    printf("%05lu %s\n", checksum, name);
  }

  return r;
}

