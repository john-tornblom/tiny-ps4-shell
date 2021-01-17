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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>

#include "_common.h"

#define SYS_nmount 378
#define MNT_UPDATE      0x0000000000010000ULL


/**
 * Code inspired by opensolaris zmount.c
 **/
static void
build_iovec(struct iovec **iov, int *iovlen, const char *name, const char *val) {
  int i;

  if (*iovlen < 0) {
    return;
  }
  
  i = *iovlen;
  *iov = realloc(*iov, sizeof(**iov) * (i + 2));
  if (*iov == NULL) {
    *iovlen = -1;
    return;
  }

  (*iov)[i].iov_base = strdup(name);
  (*iov)[i].iov_len = strlen(name) + 1;
  i++;

  (*iov)[i].iov_base = val ? strdup(val) : NULL;
  (*iov)[i].iov_len = val ? strlen(val) + 1 : 0;
  i++;  

  *iovlen = i;
}


/**
 * Split a string into an array of substrings seperated by 
 * a delimiter.
 **/
static char**
split_string(char *line, char *delim) {
  int bufsize = 64;
  int position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if(!tokens) {
    fprintf(stderr, "malloc: %s\n", strerror(errno));
    return NULL;
  }

  token = strtok(line, delim);
  while(token != NULL) {
    tokens[position] = token;
    position++;

    if(position >= bufsize) {
      bufsize *= 2;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if(!tokens) {
	fprintf(stderr, "realloc: %s\n", strerror(errno));
	free(tokens_backup);
	return NULL;
      }
    }
    
    token = strtok(NULL, delim);
  }
  tokens[position] = NULL;
  return tokens;
}


static int
mount_fs(char* fstype, char* fspath, char* device, char* options,
	 unsigned long flags) {
  struct iovec* iov = NULL;
  int iovlen = 0;
  
  build_iovec(&iov, &iovlen, "fstype", fstype);
  build_iovec(&iov, &iovlen, "fspath", fspath);

  if(device) {
    build_iovec(&iov, &iovlen, "from", device);
  }

  char **opts = split_string(options, ",");
  for(int i=0; opts[i]!=NULL; i++) {
    char *name = opts[i];
    char *value = NULL;
    char *delim = strstr(opts[i], "=");

    if(delim) {
      *delim = 0;
      value = delim+1;
    }

    build_iovec(&iov, &iovlen, name, value);
  }
  free(opts);
  
  return syscall(SYS_nmount, iov, iovlen, flags);
}


int
main_mount(int argc, char **argv) {
  char *fstype = NULL;
  char *fspath = NULL;
  char *device = NULL;
  char *options = NULL;
  unsigned long flags = 0;
  int rc = 0;
  int c;
  
  while ((c = getopt(argc, argv, "t:o:u")) != -1) {
    switch (c) {
    case 't':
      fstype = strdup(optarg);
      break;
      
    case 'o':
      options = strdup(optarg);
      break;

    case 'u':
      flags |= MNT_UPDATE;
      break;
      
    default:
      printf("usage: %s -t fstype [-u] [-o otpions] <device> <dir>\n", argv[0]);
      break;
    }
  }

  if(optind < argc) {
    device = abspath(argv[optind]);
  }

  if(optind+1 < argc) {
    fspath = abspath(argv[optind+1]);
  }

  if(device && fspath && fstype) {
    if(mount_fs(fstype, fspath, device, options, flags)) {
      perror("mount");
      rc = -1;
    }
  } else {
    fprintf(stderr, "%s: missing operand\n", argv[0]);    
  }

  if(fspath) {
    free(fspath);
  }

  if(device) {
    free(device);
  }
  
  if(fstype) {
    free(fstype);
  }

  if(options) {
    free(options);
  }

  return rc;
}

