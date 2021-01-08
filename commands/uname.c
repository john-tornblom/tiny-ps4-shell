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
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <sys/utsname.h>


typedef struct orbis_version {
  unsigned long unknown1;
  char          str_version[0x1c];
  unsigned int  bin_version;
  unsigned long unknown2;
} orbis_version_t;


void sceKernelGetSystemSwVersion(orbis_version_t *);


/**
 * 
 **/
int
main_uname(int argc, char **argv) {
  struct utsname u;
  orbis_version_t v;

  bzero(&u, sizeof(u));
  bzero(&v, sizeof(v));

#ifdef __ORBIS__
  sceKernelGetSystemSwVersion(&v);
  fprintf(stdout, "OrbisOS %s\n", v.str_version);
#else
  uname(&u);
  for(int i=0; i<5; i++) {
    char *s = ((char *) &u)+(sizeof(u.sysname)*i);
    if(s && *s) {
      printf("%s ", s);
    }
  }
  printf("\n");
#endif

  return 0;
}

