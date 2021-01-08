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


/**
 *
 **/
static void
mode_to_string(mode_t mode, char *buf) {
  char c, d;
  int i, bit;

  buf[10]=0;
  for (i=0; i<9; i++) {
    bit = mode & (1<<i);
    c = i%3;
    if (!c && (mode & (1<<((d=i/3)+9)))) {
      c = "tss"[d];
      if (!bit) c &= ~0x20;
    } else c = bit ? "xwr"[c] : '-';
    buf[9-i] = c;
  }

  if (S_ISDIR(mode)) c = 'd';
  else if (S_ISBLK(mode)) c = 'b';
  else if (S_ISCHR(mode)) c = 'c';
  else if (S_ISLNK(mode)) c = 'l';
  else if (S_ISFIFO(mode)) c = 'p';
  else if (S_ISSOCK(mode)) c = 's';
  else c = '-';
  *buf = c;
}




int
main_ls(int argc, char **argv) {
  struct dirent **namelist;
  struct stat statbuf;
  char buf[PATH_MAX];
  char path[PATH_MAX];
  char *p;
  int n, i;
  char *pwd = getenv("PWD");
  
  if(argc <= 1) {
    p = getenv("PWD");
  } else {
    p = argv[1];
  }

  if(p[0] == '/') {
    strncpy(path, p, sizeof(path));
  } else {
    snprintf(path, sizeof(path), "%s/%s", pwd, p);
  }
    
  if ((n = scandir(path, &namelist, NULL, alphasort)) < 0) {
    perror(argv[0]);
    return -1;
  }

  for(i=0; i<n; i++) {
    snprintf(buf, sizeof(buf), "%s/%s", path, namelist[i]->d_name);
    if(stat(buf, &statbuf) != 0) {
      perror(buf);
      continue;
    }
    
    mode_to_string(statbuf.st_mode, buf);
    fprintf(stdout, "%s %s\n", buf, namelist[i]->d_name);
    
    free(namelist[i]);
  }

  free(namelist);
  
  return 0;
}



