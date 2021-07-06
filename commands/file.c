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

// Code inspired by http://members.tip.net.au/%7Edbell/programs/sash-3.8.tar.gz


#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/**
 *
 **/
static const char *
check_file(const char *name)
{
  int mode;
  int fd;
  int cc;
  int i;
  int ch;
  int bad_count;
  char *cp;
  struct stat statbuf;
  char data[8192];
  static char info[1024];
  
  cp = info;
  *cp = '\0';

  if(lstat(name, &statbuf) < 0) {
    if (errno == ENOENT) {
      return "non-existent";
    }
    sprintf(cp, "stat failed: %s", strerror(errno));
    return info;
  }

  mode = statbuf.st_mode;
  if(S_ISDIR(mode)) {
    return "directory";
  }
  
  if(S_ISCHR(mode)) {
    return "character device";
  }
  
  if(S_ISBLK(mode)) {
    return "block device";
  }
  
  if(S_ISFIFO(mode)) {
    return "named pipe";
  }
  
  if(S_ISLNK(mode)) {
    return "symbolic link";
  }

  if(S_ISSOCK(mode)) {
    return "socket";
  }
  
  if(!S_ISREG(mode)) {
    sprintf(cp, "unknown mode 0x%x, \n", mode);
    cp += strlen(cp);
  }

  if((mode & (S_IEXEC | S_IXGRP | S_IXOTH)) != 0) {
    strcpy(cp, "executable, ");
    cp += strlen(cp);
  }

  if((fd = open(name, O_RDONLY)) < 0) {
    sprintf(cp, "unreadable: %s", strerror(errno));
    return info;
  }

  if((cc = read(fd, data, sizeof(data))) < 0) {
    sprintf(cp, "read error: %s", strerror(errno));
    close(fd);
    return info;
  }

  close(fd);


  if(cc == 0) {
    strcpy(cp, "empty file");
    return info;
  }

  if ((cc > 2) && (data[0] == '#') && (data[1] == '!')) {
    char * begin;
    char * end;

    data[sizeof(data) - 1] = '\0';
    begin = &data[2];

    while(*begin == ' ') {
      begin++;
    }
    end = begin;

    while(*end && (*end != ' ') && (*end != '\n')) {
      end++;
    }
    *end = '\0';
    sprintf(cp, "script for \"%s\"", begin);
    return info;
  }

  if((data[0] == '\037') && (data[1] == '\235')) {
    return "compressed file";
  }

  if((data[0] == '\037') && (data[1] == '\213')) {
    return "GZIP file";
  }
  
  if((data[0] == '\177') && (memcmp(&data[1], "ELF", 3) == 0)) {
    strcpy(cp, "ELF program");
    return info;
  }

  bad_count = 0;
  for(i = 0; i < cc; i++) {
    ch = data[i];
    if((ch == '\n') || (ch == '\t')) {
      continue;
    }
    if(isspace(ch) || isprint(ch)) {
      continue;
    }
    bad_count++;
  }

  if(bad_count != 0) {
    strcpy(cp, "binary");
    return info;
  }

  strcpy(cp, "text file");
  return info;
}


/**
 *
 **/
int
main_file(int argc, const char ** argv) {
  const char *name;
  const char *info;

  argc--;
  argv++;

  while(argc-- > 0) {
    name = *argv++;

    if (!(info = check_file(name))) {
      info = "No information available";
    }
    printf("%s: %s\n", name, info);
  }

  return 0;
}

