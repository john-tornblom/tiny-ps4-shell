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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#include <orbis/libkernel.h>
#include <orbis/SysUtil.h>

#include "kern_orbis.h"

#define BUFSIZE 1024*8

#ifndef TITLE_ID
#error missing TITLE_ID
#endif


#define INSTALL_DIR "/system/vsh/app/"TITLE_ID


static void
orbis_notify(const char *fmt, ...) {
  OrbisNotificationRequest req;
  va_list args;

  bzero(&req, sizeof req);
  va_start(args, fmt);
  vsnprintf(req.message, sizeof req.message, fmt, args);
  va_end(args);

  sceKernelSendNotificationRequest(0, &req, sizeof req, 0);
}


static char*
orbis_getcwd(char *buf, size_t size) {
#define SYS_getcwd 326

  if(syscall(SYS_getcwd, buf, size)) {
    return NULL;
  }
  return buf;
}


static void
build_iovec(struct iovec **iov, int *iovlen, const char *name, const char *val) {
  int i;

  if(*iovlen < 0) {
    return;
  }
  
  i = *iovlen;
  *iov = realloc(*iov, sizeof(**iov) * (i + 2));
  if(*iov == NULL) {
    *iovlen = -1;
    return;
  }

  (*iov)[i].iov_base = strdup(name);
  (*iov)[i].iov_len = strlen(name) + 1;
  i++;

  (*iov)[i].iov_base = val ? strdup(val) : NULL;
  (*iov)[i].iov_len = val ? strlen(val) + 1 : 0;
  i++;  

  *iovlen = i;
}


static int
mount_system() {
  struct iovec* iov = NULL;
  int iovlen = 0;

  build_iovec(&iov, &iovlen, "fstype", "exfatfs");
  build_iovec(&iov, &iovlen, "fspath", "/system");
  build_iovec(&iov, &iovlen, "from", "/dev/da0x4.crypt");
  build_iovec(&iov, &iovlen, "large", "yes");
  build_iovec(&iov, &iovlen, "timezone", "static");
  build_iovec(&iov, &iovlen, "async", NULL);
  build_iovec(&iov, &iovlen, "ignoreacl", NULL);

#define SYS_nmount 378
#define MNT_UPDATE 0x0000000000010000ULL
  
  return syscall(SYS_nmount, iov, iovlen, MNT_UPDATE);
}


static int
copy_file(const char* src_str, const char *dst_str) {
  struct stat s;
  int src;
  int dst;

  if (!(src = open(src_str, O_RDONLY, 0))) {
    return -1;
  }

  if(!(dst = open(dst_str, O_CREAT | O_WRONLY, 0755))) {
    close(src);
    return -1;
  }

  bzero(&s, sizeof s);
  fstat(src, &s);
  if(!s.st_size) {
    close(src);
    close(dst);
    return -1;
  }

  char data[s.st_size];
  if(read(src, data, s.st_size) != s.st_size) {
    close(src);
    close(dst);
    return -1;
  }

  if(write(dst, data, s.st_size) != s.st_size) {
    close(src);
    close(dst);
    return -1;
  }

  close(src);
  close(dst);

  return 0;
}


static void
install() {
  char cwd[PATH_MAX];
  char path[PATH_MAX];
  uint64_t attrs = app_get_attributes();
  uint64_t caps = app_get_capabilities();

  app_set_attributes(attrs | (1ULL << 62));
  app_set_capabilities(caps | (1ULL << 62));

  if(app_jailbreak()) {
    orbis_notify("Unable to escape jail");
    return;
  }

  if(!orbis_getcwd(cwd, sizeof cwd)) {
    orbis_notify("Unable to obtain current working directory");
    return;
  } 

  if(seteuid(0)) {
    orbis_notify("Unable to acquire root permissions");
    return;
  }

  if(mount_system()) {
    orbis_notify("Unable to remount root partition");
    return;
  }

  if(mkdir(INSTALL_DIR, 0755) && errno != EEXIST) {
    orbis_notify("Unable to create install dir");
    return;
  }

  if(mkdir(INSTALL_DIR"/sce_sys", 0755) && errno != EEXIST) {
    orbis_notify("Unable to create install dir");
    return;
  }

  snprintf(path, sizeof path, "%s/app0/daemon/eboot.bin", cwd);
  if(copy_file(path, INSTALL_DIR"/eboot.bin")) {
    orbis_notify("Unable to install %s", path);
    return;
  }

  snprintf(path, sizeof path, "%s/app0/daemon/param", cwd);
  if(copy_file(path, INSTALL_DIR"/sce_sys/param.sfo")) {
    orbis_notify("Unable to install %s", path);
    return;
  }

  orbis_notify("Install successful");
}


int main() {
  install();
  
  while(1) {
    sleep(1);
  }

  return 0;
}

