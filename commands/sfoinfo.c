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

// Code derived from https://github.com/TheOfficialFloW/VitaShell

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "sfo.h"
#include "_common.h"


static char*
freadstr(FILE *fp) {
  int size = 64;
  int pos = 0;
  char *tmp;
  char *buf;
  char c;

  if(!(buf = calloc(size, sizeof(char)))) {
    fprintf(stderr, "malloc: %s\n", strerror(errno));
    return NULL;
  }

  while(1) {
    if((c = fgetc(fp)) <= 0) {
      buf[pos] = '\0';
      return buf;
    }
  
    buf[pos++] = c;
    if(pos >= size) {
      size *= 2;
      tmp = buf;
      if(!(buf = realloc(buf, size))) {
	fprintf(stderr, "realloc: %s\n", strerror(errno));
	free(tmp);
	return NULL;
      }
    }
  }
}


static int
sfoinfo(FILE *fp) {
  sfo_header_t header;

  if(fread(&header, sizeof(sfo_header_t), 1, fp) < 1) {
    return -1;
  }

  if(header.magic != MAGIC) {
    return -1;
  }

  if(!header.count) {
    return 0;
  }

  sfo_entry_t entries[header.count];
  if(fread(&entries, sizeof(sfo_entry_t), header.count, fp) < 1) {
    return -1;
  }
    
  for(int i=0; i<header.count; i++) {
    sfo_entry_t *e = &entries[i];
    uint8_t val[e->val_size+1];
    char *key;
    
    if(fseek(fp, header.keys_offset + e->key_offset, SEEK_SET)) {
      return -1;
    }

    if(!(key = freadstr(fp))) {
      return -1;      
    }
    
    if(fseek(fp, header.data_offset + e->val_offset, SEEK_SET)) {
      free(key);
      return -1;
    }

    memset(val, 0, sizeof(val));
    if(fread(val, sizeof(uint8_t), e->val_size, fp) != e->val_size) {
      free(key);
      return -1;      
    }

    printf("%s(", key);
    switch(e->type) {
    case TYPE_INT:
      printf("0x%02x%02x%02x%02x", val[3], val[2], val[1], val[0]);
      break;
      
    case TYPE_STR:
      printf("'%s'", val);
      break;
      
    default:
      printf("%d, 0x", e->type);
      for(int i=0; i<e->val_length; i++) {
	printf("%02x", val[i]);
      }
      break;
    }
    printf(")\n");

    free(key);
  }

  return 0;
}


int
main_sfoinfo(int argc, char **argv) {
  FILE *fp;

  if(argc <= 1) {
    fprintf(stderr, "usage: %s param.sfo\n", argv[0]);
    return -1;
  }

  for(int i=1; i<argc; i++) {
    char *path = abspath(argv[i]);
    if(!(fp = fopen(argv[i], "rb"))) {
      perror(argv[i]);
    } else {
      sfoinfo(fp);
    }
    free(path);
  }

  return 0;
}

