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

#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/**
 * Dump a memory region to stdout.
 **/
void
hexdump(void *data, size_t size) {
  char* p = (char*)data;

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
      printf("\n");
    }
    printf("%02X ", p[i]);
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

/**
 * Return the current working directory of the calling process.
 **/
char*
get_workdir(void) {
  //getcwd() does not seem to be not working on the PS4
  return getenv("PWD");
}



/**
 * Normalize path by eliminating double slashes.
 **/
char*
normpath(const char *path) {
  //TODO
  return strdup(path);
}


/**
 * Return an absolute path.
 **/
char *
abspath(const char *relpath) {
  char ap[PATH_MAX];
  
  if(relpath[0] == '/') {
    return strdup(relpath);
  }

  snprintf(ap, sizeof(ap), "%s/%s", get_workdir(), relpath);

  return normpath(ap);
}

