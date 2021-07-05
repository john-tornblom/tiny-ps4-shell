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

// Code inspired by https://github.com/landley/toybox

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#include "_common.h"


/**
 *
 **/
static const char *
build_name(const char * dir_name, const char * file_name) {
  const char *cp;
  static char buf[PATH_MAX];

  if ((dir_name == NULL) || (*dir_name == '\0')) {
    return file_name;
  }

  if((cp = strrchr(file_name, '/'))) {
    file_name = cp + 1;
  }

  strcpy(buf, dir_name);
  strcat(buf, "/");
  strcat(buf, file_name);

  return buf;
}


int
main_ln(int argc, char **argv) {
  const char *src_name;
  const char *dest_name;
  const char *last_arg;
  struct stat statbuf;
  int is_dir;
  int r;

  if(argc <= 1) {
      fprintf(stderr, " missing file operand\n");
      return 1;
  }
  r = 0;

  // soft link
  if(argv[1][0] == '-') {
    if(strcmp(argv[1], "-s")) {
      fprintf(stderr, "Unknown option\n");
      return 1;
    }

    if (argc != 4) {
      fprintf(stderr, "Wrong number of arguments for symbolic link\n");
      return 1;
    }

    if(symlink(argv[2], argv[3]) < 0) {
      perror(argv[3]);
      return 1;
    }
    return 0;
  }

  // hard link
  last_arg = argv[argc - 1];
  is_dir = (!stat(last_arg, &statbuf) &&
	    S_ISDIR(statbuf.st_mode));

  if ((argc > 3) && !is_dir) {
    fprintf(stderr, "%s: not a directory\n", last_arg);
    return 1;
  }

  while (argc-- > 2) {
    src_name = *(++argv);
    if(access(src_name, 0) < 0) {
      perror(src_name);
      r = 1;
      continue;
    }
    
    dest_name = last_arg;

    if(is_dir) {
      dest_name = build_name(dest_name, src_name);
    }
    
    if(link(src_name, dest_name) < 0) {
      perror(dest_name);
      r = 1;
      continue;
    }
  }
  
  return r;
}



