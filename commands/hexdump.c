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


static void
hexdump(void *data, size_t size) {
  char* p = (char*)data;

  printf("0x%08x  ", 0);
  for(int i=0; i<size; i++) {    
    if(i && i%16 == 0) {
      printf("| ");
      for(int j=0; j<16; j++) {
	char ch = p[i-16+j];

	if(isblank(ch)) {
	  printf(" ");
	  
	} else if(isprint(ch)) {
	  printf("%c", ch);
	}
      }
      printf("\n0x%08x  ", i);
    }
    printf("%02X ", (uint8_t)p[i]);
  }

  int mod = size % 16;
  for(int i=mod; i<16; i++) {
    printf("   ");
  }

  printf("| ");
  for(int i=0; i<mod; i++) {
    char ch = p[size-mod+i];

    if(isblank(ch)) {
      printf(" ");
      
    } else if(isprint(ch)) {
      printf("%c", ch);
    }
  }
  printf("\n");
  
}


int main_hexdump(int argc, char **argv) {
  FILE *fp;
  int size;
  void *buf;
  
  if(argc <= 1) {
    fprintf(stderr, "%s: missing operand\n", argv[0]);
    return -1;
  }

  char *path = abspath(argv[1]);
  fp = fopen(path, "rb");
  free(path);
  
  if(!fp) {
    perror(argv[1]);
    return -1;
  }

  if(fseek(fp, 0, SEEK_END)) {
    perror(argv[1]);
    return -1;
  }

  if((size = ftell(fp)) < 0) {
    perror(argv[1]);
    return -1;
  }
  
  rewind(fp);

  if(!(buf = malloc(size))) {
    perror(argv[1]);
    return -1;
  }

  if(fread(buf, sizeof(char), size, fp) < 0) {
    perror(argv[1]);
    return -1;
  }

  hexdump(buf, size);
  free(buf);

  return 0;
}

