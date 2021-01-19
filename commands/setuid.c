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
#include <limits.h>

#include <sys/types.h>
#include <unistd.h>

#include "kern_orbis.h"


int
main_setuid(int argc, char **argv) {
  if(argc <= 1) {
    printf("%s: missing operand\n", argv[0]);
    return -1;
  }
  
#ifdef __ORBIS__
  uint64_t attrs = app_get_attributes();
  uint64_t caps = app_get_capabilities();
  
  app_set_attributes(attrs | (1ULL << 62));
  app_set_capabilities(caps | (1ULL << 62));
  
  setuid(atoi(argv[1]));

  app_set_attributes(attrs);
  app_set_capabilities(caps);
#else
  setuid(atoi(argv[1]));  
#endif

  return 0;
}



