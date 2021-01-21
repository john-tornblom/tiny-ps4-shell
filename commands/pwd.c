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
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "sys.h"


int
main_pwd(int argc, char **argv) {
  char pwd[PATH_MAX];
  pwd[0] = 0;

  if(!sys_getcwd(pwd, sizeof pwd)) {
    perror(argv[0]);
    return EXIT_FAILURE;
  } else {
    printf("%s\n", pwd);
  }

  return EXIT_SUCCESS;
}


