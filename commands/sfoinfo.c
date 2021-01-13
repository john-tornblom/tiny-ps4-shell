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


typedef struct sfo_header {
  uint32_t magic;
  uint32_t version;
  uint32_t keys_offset;
  uint32_t data_offset;
  uint32_t count;
} __attribute__((packed)) sfo_header_t;


typedef struct sfo_entry {
  uint16_t key_offset;
  uint8_t  alignment;
  uint8_t  type;
  uint32_t val_size;
  uint32_t tot_size;
  uint32_t val_offset;
} __attribute__((packed)) sfo_entry_t;


#define SFO_MAGIC 0x46535000

#define PSF_TYPE_BIN 0
#define PSF_TYPE_STR 2
#define PSF_TYPE_INT 4


static int
sfoinfo_dump(const char *filename) {
  sfo_header_t header;
  FILE *fp;
  
  if(!(fp = fopen(filename, "rb"))) {
    perror(filename);
    return -1;
  }

  if(fread(&header, sizeof(sfo_header_t), 1, fp) < 1) {
    fprintf(stderr, "%s: Unexpected end of file\n", filename);
    return -1;
  }

  if(header.magic != SFO_MAGIC) {
    fprintf(stderr, "%s: Unexpected magic number\n", filename);
    return -1;
  }

  sfo_entry_t entries[header.count];
  if(fread(&entries, sizeof(sfo_entry_t), header.count, fp) < 1) {
    fprintf(stderr, "%s: Unexpected end of file\n", filename);
    return -1;
  }
    
  for(int i=0; i<header.count; i++) {
    sfo_entry_t *e = &entries[i];
    uint8_t val[e->tot_size];
    char key[64];

    if(fseek(fp, header.keys_offset + e->key_offset, SEEK_SET)) {
      perror(filename);
      return -1;
    }
    
    if(fread(key, sizeof(char), 64, fp) != 64) {
      fprintf(stderr, "%s: Unexpected end of file\n", filename);
      return -1;      
    }
    
    if(fseek(fp, header.data_offset + e->val_offset, SEEK_SET)) {
      perror(filename);
      return -1;
    }
    
    if(fread(val, sizeof(uint8_t), e->tot_size, fp) != e->tot_size) {
      fprintf(stderr, "%s: Unexpected end of file\n", filename);
      return -1;      
    }

    printf("%s(", key);
    switch(e->type) {
    case PSF_TYPE_INT:
      printf("0x%02x%02x%02x%02x", val[3], val[2], val[1], val[0]);
      break;
      
    case PSF_TYPE_STR:
      printf("'%s'", val);
      break;
      
    default:
      printf("0x");
      for(int i=0; i<e->tot_size; i++) {
	printf("%02x", val[i]);
      }
      break;
    }
    printf(")\n");
  }

  return 0;
}


int main_sfoinfo(int argc, char **argv) {

  if(argc <= 1) {
    fprintf(stderr, "%s: missing operand\n", argv[0]);
    return -1;
  }

  return sfoinfo_dump(argv[1]);
}
