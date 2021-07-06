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

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <utime.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>


#define BUF_SIZE 1024*8


/**
 * state variable for catching user interupt signals
 **/
static int interupted = 0;


/**
 * Catch user interupts
 **/
static void
on_SIGINT(int val) {
  interupted = 1;
}


/**
 * Report error in case status is -1.
 **/
static void
check_status(const char * name, int status) {
  if (status == -1) {
    perror(name);
  }
}


/**
 *
 **/
static int
full_write(int fd, const char * buf, int len) {
  int cc;
  int total;

  total = 0;

  while(len > 0) {
    cc = write(fd, buf, len);

    if(cc < 0) {
      return -1;
    }
    buf += cc;
    total += cc;
    len -= cc;
  }

  return total;
}


/**
 *
 **/
static const char *
build_name(const char * dir_name, const char * file_name) {
  const char *cp;
  static char buf[PATH_MAX];

  if ((dir_name == NULL) || (*dir_name == '\0')) {
    return file_name;
  }

  if((cp = strrchr(file_name, '/'))) {
    file_name = cp + 1;
  }

  strcpy(buf, dir_name);
  strcat(buf, "/");
  strcat(buf, file_name);

  return buf;
}


static int
copy_file(const char *src_name, const char *dest_name, int set_modes) {
  int rfd;
  int wfd;
  int rcc;
  char buf[BUF_SIZE];
  struct stat statbuf1;
  struct stat statbuf2;
  struct utimbuf times;
  
  if(stat(src_name, &statbuf1) < 0) {
    perror(src_name);
    return -1;
  }

  if(stat(dest_name, &statbuf2) < 0) {
    statbuf2.st_ino = -1;
    statbuf2.st_dev = -1;
  }

  if((statbuf1.st_dev == statbuf2.st_dev) &&
     (statbuf1.st_ino == statbuf2.st_ino)) {
    fprintf(stderr, "Copying file \"%s\" to itself\n", src_name);
    return -1;
  }

  if((rfd = open(src_name, O_RDONLY)) < 0) {
    perror(src_name);
    return -1;
  }
  
  if((wfd = creat(dest_name, statbuf1.st_mode)) < 0) {
    perror(dest_name);
    close(rfd);
    return -1;
  }

  while((rcc = read(rfd, buf, sizeof(buf))) > 0) {
    if(interupted) {
      close(rfd);
      close(wfd);
      return -1;
    }

    if(full_write(wfd, buf, rcc) < 0) {
      goto error_exit;
    }
  }

  if(rcc < 0) {
    perror(src_name);
    goto error_exit;
  }
  
  check_status("close", close(rfd));
  if(close(wfd) < 0) {
    perror(dest_name);
    return -1;
  }

  if(set_modes) {
    check_status("chmod", chmod(dest_name, statbuf1.st_mode));
    check_status("chown", chown(dest_name, statbuf1.st_uid, statbuf1.st_gid));

    times.actime = statbuf1.st_atime;
    times.modtime = statbuf1.st_mtime;
    
    check_status("utime", utime(dest_name, &times));
  }

  return 0;

error_exit:
  close(rfd);
  close(wfd);

  return -1;
}



/**
 *
 **/
int
main_mv(int argc, const char ** argv) {
  const char * src_name;
  const char * dest_name;
  const char * last_arg;
  struct stat statbuf;
  int is_dir;
  int r;

  signal(SIGINT, on_SIGINT);
  
  r = 0;
  last_arg = argv[argc - 1];

  is_dir = (!stat(last_arg, &statbuf) &&
	    S_ISDIR(statbuf.st_mode));

  if((argc > 3) && !is_dir) {
    fprintf(stderr, "%s: not a directory\n", last_arg);
    return 1;
  }

  while(!interupted && (argc-- > 2)) {
    src_name = *(++argv);

    if (access(src_name, 0) < 0) {
      perror(src_name);
      r = 1;
      continue;
    }

    dest_name = last_arg;

    if(is_dir) {
      dest_name = build_name(dest_name, src_name);
    }
    
    if(rename(src_name, dest_name) >= 0) {
      continue;
    }

    if (errno != EXDEV) {
      perror(dest_name);
      r = 1;
      continue;
    }

    if(!copy_file(src_name, dest_name, 1)) {
      r = 1;
      continue;
    }

    if(unlink(src_name) < 0) {
      perror(src_name);
      r = 1;
    }
  }
  
  return r;
}

