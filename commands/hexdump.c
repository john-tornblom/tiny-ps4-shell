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

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "_common.h"


static int
fgetb(uint8_t *buf, size_t size, FILE *fp) {
  int c;
  
  for(int i=0; i<size; i++) {
    if((c = fgetc(fp)) < 0) {
      return i;
    }
    buf[i] = c;
  }

  return size;
}


static void
hexdump_file(FILE *fp) {
  uint8_t buf[16];
  int size = 0;
  int n;
  
  while((n = fgetb(buf, sizeof(buf), fp))) {
    printf("%08x  ", size);

    for(int i=0; i<n; i++) {
      printf("%02x ", buf[i]);
    }

    for(int i=n; i<sizeof(buf); i++) {
      printf("   ");
    }

    printf("| ");
    
    for(int i=0; i<n; i++) {
      if(isblank(buf[i])) {
	printf(" ");
	
      } else if(!isprint(buf[i])) {
	printf(".");
	
      } else {
	printf("%c", buf[i]);
      }
    }
    printf("\n");
    size += n;
  }
}


int main_hexdump(int argc, char **argv) {
  FILE *fp = stdin;
  
  if(argc > 1) {
    char *path = abspath(argv[1]);

    if(!(fp = fopen(path, "rb"))) {
      perror(argv[1]);
      return -1;
    }
    free(path);    
  }

  hexdump_file(fp);

  return 0;
}

