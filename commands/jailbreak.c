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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include "kern_orbis.h"


#define AUTH_MAX 0x3801000000000013ULL
#define ATTR_MAX 0xFFFFFFFFFFFFFFFFULL
#define CAPS_MAX 0xFFFFFFFFFFFFFFFFULL


/**
 * Escape application sandbox and raise privileges.
 **/
int
main_jailbreak(int argc, char **argv) {
#ifdef __ORBIS__
  printf("\n\nWIP\n\n");
  printf("The jailbreak command has been disable since it is currently\n");
  printf("crashing the PS4 when the parent application is terminated\n");

  /*
  app_set_authid(AUTH_MAX);
  app_set_attributes(ATTR_MAX);
  app_set_capabilities(CAPS_MAX);
  app_jailbreak();
  
  seteuid(0);
  setegid(0);
  */
  
#endif
  
  return 0;
}
