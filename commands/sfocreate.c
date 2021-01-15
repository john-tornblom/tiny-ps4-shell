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


#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "sfo.h"
#include "_common.h"


typedef struct keyval {
  char *key;
  void *val;
  uint8_t type;
  size_t key_size;
  size_t val_size;
  size_t val_length;
  struct keyval *next;
} keyval_t;


/**
 * Global state variables.
 **/
static keyval_t* keyval_store = NULL;
static size_t count = 0;
static size_t keys_size = 0;
static size_t data_size = 0;


/**
 * Insert a key-value in the global store.
 **/
static void
keyval_store_insert(keyval_t *kv) {
  keyval_t *next = keyval_store;
  keyval_t *prev = NULL;
  
  while(next && strcmp(next->key, kv->key) < 0) {
    prev = next;
    next = next->next;
  }

  if(prev) {
    kv->next = prev->next;
    prev->next = kv;
    
  } else {
    kv->next = keyval_store;
    keyval_store = kv;
  }

  keys_size += kv->key_size;
  data_size += kv->val_size;
  count += 1;
}


/**
 * Emit the key-value store to a file.
 **/
static void
keyval_store_emit(FILE *fp) {
  size_t keys_offset = 0;
  size_t data_offset = 0;
  keyval_t *kv = keyval_store;
  uint8_t data[data_size];
  uint8_t keys[keys_size];
  sfo_entry_t entries[count];
  sfo_header_t header = {
    .magic = 0x46535000,
    .version = 0x0101,
    .count = count,
    .keys_offset = sizeof(sfo_header_t) + sizeof(entries),
    .data_offset = sizeof(sfo_header_t) + sizeof(entries) + sizeof(keys)
  };
  
  memset(keys, 0, keys_size);
  memset(data, 0, data_size);
  
  for(int i=0; i<count; i++) {
    memcpy(keys + keys_offset, kv->key, kv->key_size);
    memcpy(data + data_offset, kv->val, kv->val_size);
    
    entries[i].key_offset = keys_offset;
    entries[i].alignment = 4;
    entries[i].type = kv->type;
    entries[i].val_length = kv->val_length;
    entries[i].val_size = kv->val_size;
    entries[i].val_offset = data_offset;

    keys_offset += kv->key_size;
    data_offset += kv->val_size;
    
    kv = kv->next;
  }
  
  fwrite(&header, sizeof(sfo_header_t), 1, fp);
  fwrite(entries, sizeof(sfo_entry_t), count, fp);
  fwrite(keys, sizeof(uint8_t), keys_size, fp);
  fwrite(data, sizeof(uint8_t), data_size, fp);
}


/**
 * Parse a key-value integer of the format KEY=VALUE.
 **/
static int
keyval_parse_integer(char* str) {
  keyval_t* kv = NULL;
  char *delim = NULL;
  char *key = str;
  int val = 0;
  
  if(!(delim = strstr(str, "="))) {
    return -1;
  }

  *delim = 0;
  val = atoi(delim + 1);
  
  kv = malloc(sizeof(keyval_t));
  kv->type = TYPE_INT;

  kv->key = strdup(key);
  kv->key_size = strlen(key) + 1;

  kv->val_size = 4;
  kv->val_length = 4;
  kv->val = malloc(kv->val_size);
  memcpy(kv->val, &val, kv->val_size);

  keyval_store_insert(kv);

  return 0;
}


/**
 * Parse a key-value string of the format KEY=VALUE.
 **/
static int
keyval_parse_string(char* str, size_t size) {
  keyval_t* kv = NULL;
  char *delim = NULL;
  char *key = str;
  char *val = NULL;
  
  if(!(delim = strstr(str, "="))) {
    return -1;
  }

  *delim = 0;
  val = delim + 1;
  
  kv = malloc(sizeof(keyval_t));
  kv->type = TYPE_STR;

  kv->key = strdup(key);
  kv->key_size = strlen(kv->key) + 1;

  kv->val = malloc(size);
  kv->val_size = size;
  kv->val_length = strnlen(val, size) + 1;

  strncpy(kv->val, val, kv->val_size);

  keyval_store_insert(kv);

  return 0;
}


/**
 * Display usage help text and terminate program.
 **/
static void
show_help(const char *progname) {
  printf("usage: %s [-i KEY=VALUE] [-s<SIZE> KEY=VALUE]... param.sfo\n",
	 progname);
  exit(1);
}


int
main_sfocreate(int argc, char **argv) {
  char* filename = NULL;
  FILE *fp = stdout;
  char *optarg = NULL;
  uint32_t size = 0;
  
  for(int i=1; i<argc; i++) {
    if(i < argc - 1) {
      optarg = strdup(argv[i+1]);
    } else {
      optarg = NULL;
    }
    
    if(argv[i][0] != '-') {
      if(filename) {
	free(filename);
      }
      filename = abspath(argv[i]);

    } else if(!strcmp(argv[i], "-h")) {
      show_help(argv[0]);

    } else if(!strcmp(argv[i], "-i")) {
      if(!optarg || keyval_parse_integer(optarg)) {
	fprintf(stderr, "%s integer key-value pair\n",
		optarg ? "Malformed" : "Missing");
	return -1;
      }
    } else if(sscanf(argv[i], "-s%u", &size) == 1) {
      if(!optarg || keyval_parse_string(optarg, size)) {
	fprintf(stderr, "%s string key-value pair\n",
		optarg ? "Malformed" : "Missing");
	return -1;
      }
    } else {
      fprintf(stderr, "Invalid option %s\n", argv[i]);
      return -1;
    }

    if(optarg) {
      free(optarg);
    }
  }

  if(filename && !(fp = fopen(filename, "wb"))) {
    perror(filename);
    return -1;
  }
  
  keyval_store_emit(fp);

  if(filename) {
    free(filename);
  }
  
  return 0;
}

