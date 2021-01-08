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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mount.h>
#include <string.h>


int
main_mount(int argc, char **argv) {
  char *type = NULL;
  char *options = NULL;
  char *device = NULL;
  char *dir = NULL;
  unsigned long flags = 0;
  int rc = 0;
  int c;
  
  while ((c = getopt(argc, argv, "t:o:")) != -1) {
    switch (c) {
    case 't':
      type = strdup(optarg);
      break;
      
    case 'o':
      options = strdup(optarg);
      break;

    default:
      printf("usage: %s -t fstype [-o otpions] <device> <dir>\n", argv[0]);
      break;
    }
  }

  if(optind < argc) {
    device = argv[optind];
  }

  if(optind+1 < argc) {
    dir = argv[optind+1];
  }

  if(device && dir && type) {
    if(mount(device, dir, type, flags, NULL)) {
      perror("mount");
      rc = -1;
    }
  } else {
    fprintf(stderr, "%s: missing operand\n", argv[0]);    
  }
  
  if(type) {
    free(type);
  }

  if(options) {
    free(options);
  }

  return rc;
}

