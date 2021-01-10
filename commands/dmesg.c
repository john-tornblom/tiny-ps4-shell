#include <stdio.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


static void
logline(FILE *outfp, char *proc, char *line) {
  time_t now;
  struct tm *tm;
  char *d, *s;

  time(&now);
  tm = localtime(&now);
  d = asctime(tm);

  if((s=strrchr(d, ' '))) {
    *s = '\0';
  }
  
  if((s=strchr(d, ' '))) {
    d = s+1;
  }
  
  fprintf(outfp, "%s OrbisOS: %s\n", d, line);
}


void logstream(int infd, FILE *outfp) {
  static char linebuf[5*1024];
  int l, acc = 0;
  
  while((l = read(infd, linebuf, sizeof(linebuf)-2)) > 0) {
    char *b, *eol;
    int i;
    acc += l;
    for(i = 0; i < l; i++) {
      if(linebuf[i] == '\0') {
	linebuf[i] = ' ';
      }
    }
    
    if(linebuf[l-1] == '\n') {
      l--;
    }
    
    linebuf[l] = '\n';
    linebuf[l+1] = '\0';
    b = linebuf;

    while((eol = strchr(b, '\n'))) {
      *eol = '\0';
      logline(outfp, "kernel", b);
      b = eol+1;
    }
  }

  if(!acc) {
    sleep(1);
  }

  return;
}


int
main_dmesg(int argc, char **argv) {
  int fd;
  fd_set fds;
  struct timeval timeout;
  
  id_t euid = geteuid();
  seteuid(0);
  if((fd = open("/dev/klog", O_NONBLOCK | O_RDONLY)) < 0) {
    perror("/dev/klog");
    return 1;
  }
  seteuid(euid);
  
  FD_ZERO(&fds);
  FD_SET(fd, &fds);

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  
  if(select(fd+1, &fds, NULL, NULL, &timeout) > 0) {
    if(FD_ISSET(fd, &fds)) {
      logstream(fd, stdout);
    }
    fflush(stdout);
    sync();
  }

  return 0;
}
