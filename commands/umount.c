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
#include <unistd.h>

#include "_common.h"

#ifdef __ORBIS__
#define SYS_unmount 22

#define MNT_FORCE       0x0000000000080000ULL


static int
umount2(const char *path, uint64_t flags) {
  return syscall(SYS_unmount, path, flags);
}

#else
#include <sys/mount.h>
#endif

int
main_umount(int argc, char **argv) {
  uint64_t flags = 0;
  char *path = NULL;
  int c;
  
  while ((c = getopt(argc, argv, "fh")) != -1) {
    switch (c) {
    case 'f':
      flags |= MNT_FORCE;
      break;

    case 'h':
    default:
      printf("usage: %s [-f] <dir>\n", argv[0]);
      exit(1);
      break;
    }
  }

  if(optind < argc) {
    path = abspath(argv[optind]);
  } else {
    fprintf(stderr, "%s: missing operand\n", argv[0]);
    return -1;
  }

  if(umount2(path, flags)) {
    perror(path);
  }
    
  free(path);

  return 0;
}


