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
#include <sys/stat.h>


#define is_octal(ch) (((ch) >= '0') && ((ch) <= '7'))

/**
 *
 **/
int
main_chmod(int argc, const char ** argv) {
  const char *cp;
  int mode;
  int r;

  r = 0;
  mode = 0;
  cp = argv[1];

  while(is_octal(*cp))
    mode = mode * 8 + (*cp++ - '0');

  if(*cp) {
    fprintf(stderr, "Mode must be octal\n");
    return 1;
  }
  
  argc--;
  argv++;
  
  while(argc-- > 1) {
    if(chmod(argv[1], mode) < 0) {
      perror(argv[1]);
      r = 1;
    }    
    argv++;
  }
  
  return r;
}

