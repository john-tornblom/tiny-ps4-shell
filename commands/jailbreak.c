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
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include "sys.h"
#include "kern_orbis.h"


/**
 * Escape application sandbox and raise privileges.
 **/
int
main_jailbreak(int argc, char **argv) {
  char cwd[PATH_MAX];
  
#ifdef __ORBIS__
  app_jailbreak();
#endif

  setenv("OLDPWD", getenv("PWD"), 1);
  setenv("PWD", sys_getcwd(cwd, sizeof cwd), 1);
  
  if(seteuid(0)) {
    perror("seteuid");
  }
  
  return 0;
}
