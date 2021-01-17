#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <ctype.h>


typedef struct proc_status {
  char *cmd;
  pid_t pid;
  pid_t ppid;
  pid_t pgid;
  pid_t sid;
  char *tty;
} proc_status_t;


static char*
next_col(char **ptr) {
  char *p = *ptr;
  char *d = strstr(p, " ");
  if(d) {
    *d = 0;
    *ptr = d + 1;
  } else {
    *ptr = 0;
  }
  
  return p;
}


static int
digitsort(const struct dirent **a, const struct dirent **b) {
  return atoi((*a)->d_name) - atoi((*b)->d_name);
}


static void
parse_proc_status(char *status, proc_status_t *ps) {
  char *ptr = status;
  
  ps->cmd = next_col(&ptr);
  ps->pid = atoi(next_col(&ptr));
  ps->ppid = atoi(next_col(&ptr));
  ps->pgid = atoi(next_col(&ptr));
  ps->sid = atoi(next_col(&ptr));
  ps->tty = next_col(&ptr);
}


int
main_ps(int argc, char** argv) {
  struct dirent **namelist;
  struct stat statbuf;
  char path[PATH_MAX];
  char buf[1000];
  proc_status_t ps;
  FILE *fp;
  int n;

  if ((n = scandir("/proc", &namelist, NULL, digitsort)) < 0) {
    perror(argv[0]);
    return -1;
  }
  
  printf("     PID      PPID     PGID      SID    TTY    COMMAND\n");

  for(int i=0; i<n; i++) {
    if(!isdigit(namelist[i]->d_name[0])) {
      continue;
    }
    
    snprintf(path, sizeof(path), "/proc/%s/status", namelist[i]->d_name);
    if(stat(path, &statbuf) != 0) {
      perror(path);
      continue;
    }
    
    if(!(fp = fopen(path, "r"))) {
      perror(path);
      continue;
    }

    if(fread(buf, sizeof(char), sizeof(buf), fp) <= 0) {
      perror(path);
      continue;
    }
    
    parse_proc_status(buf, &ps);
    printf("%8d  %8d %8d %8d %6s    %s\n",
	   ps.pid, ps.ppid, ps.pgid, ps.sid, ps.tty, ps.cmd);  
  }
  
  return 0;
}

