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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>


static char*
normpath(const char *path) {
  return strdup(path);
}


static char *
abspath(const char *path) {
  char ap[PATH_MAX];
  
  if(path[0] == '/') {
    return strdup(path);
  }

  snprintf(ap, sizeof(ap), "%s/%s", getenv("PWD"), path);

  return normpath(ap);
}


/**
 * 
 **/
int
main_cd(int argc, char **argv) {
  char *old = strdup(getenv("PWD"));
  char *new = NULL;
  int exit_code;
  
  if(argc <= 1) {
    new = getenv("HOME");
    
  } else if (!strcmp(argv[1], "-")) {
    new = getenv("OLDPWD");
    
  } else {
    new = argv[1];
  }

  if(!new[0]) {
    new = "/";
  }

  new = abspath(new);
  
  setenv("PWD", new, 1);
  setenv("OLDPWD", old, 1);
  
  free(old);
  free(new);
  
  return exit_code;
}

