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

#include "_common.h"


void cat(FILE *fp) {
  int c;
  while((c = getc(fp)) > 0) {
    putc(c, stdout);
  }
}


/**
 * 
 **/
int
main_cat(int argc, char **argv) {
  for(int i=1; i<argc; i++) {
    FILE *fp;
    char *path = abspath(argv[i]);
    
    if(!(fp = fopen(path, "r"))) {
      perror(argv[i]);
    } else {
      cat(fp);
    }
    
    free(path);
  }

  if(argc <= 1) {
    cat(stdin);
  }
  
  return 0;
}

