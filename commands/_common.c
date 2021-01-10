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


#define ispathsep(ch)   ((ch) == '/' || (ch) == '\\')
#define iseos(ch)       ((ch) == '\0')
#define ispathend(ch)   (ispathsep(ch) || iseos(ch))


/**
 * Normalize a path.
 *
 * Inspired by https://gist.github.com/starwing/2761647
 **/
char *normpath(const char *in, char *buf, size_t bufsize) {
  char *pos[PATH_MAX];
  char **top = pos;
  char *head = buf;
  int isabs = ispathsep(*in);

  if(isabs && bufsize) {
    *buf++ = '/';
    bufsize--;
  }
  
  *top++ = buf;

  while(!iseos(*in)) {
    while(ispathsep(*in)) {
      ++in;
    }
    
    if(iseos(*in)) {
      break;
    }
    
    if(memcmp(in, ".", 1) == 0 && ispathend(in[1])) {
      ++in;
      continue;
    }

    if(memcmp(in, "..", 2) == 0 && ispathend(in[2])) {
      in += 2;
      
      if(top != pos + 1) {
	buf = *--top;
	
      } else if(isabs) {
	buf = top[-1];
	
      } else {
	strncpy(buf, "../", bufsize);
	buf += 3;
	bufsize -= 3;
      }
      
      continue;
    }

    if(top - pos >= PATH_MAX) {
      return NULL;
    }
    
    *top++ = buf;
    
    while(!ispathend(*in) && bufsize) {
      *buf++ = *in++;
      bufsize--;
    }
    
    if(ispathsep(*in) && bufsize) {
      *buf++ = '/';
      bufsize--;
    }
  }

  *buf = '\0';
  
  if(*head == '\0') {
    strcpy(head, "./");
  }
  
  return head;
}


/**
 * Return an absolute path.
 **/
char *
abspath(const char *relpath) {
  char buf[PATH_MAX];
  
  if(relpath[0] == '/') {
    strncpy(buf, relpath, sizeof(buf));
  } else {
    snprintf(buf, sizeof(buf), "%s/%s", get_workdir(), relpath);
  }
  
  char *ap = malloc(PATH_MAX);
  return normpath(buf, ap, PATH_MAX);
}

