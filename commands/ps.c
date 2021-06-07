#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>


struct kinfo_proc {
  char junk0[0x48];
  pid_t	pid;
  pid_t	ppid;
  pid_t	pgid;
  pid_t	tpgid;
  pid_t	sid;
  char junk1[0x163];
  char name[19];
};


int sysctl(const int *name, u_int namelen, void *oldp,	size_t *oldlenp,
	   const void *newp, size_t newlen);


int
main_ps(int argc, char** argv) {

  int i, mib[4] = {1, 14, 1, 0};
  size_t len;
  char buf[10000];


  printf("     PID      PPID     PGID      SID   COMMAND\n");
  for(i=0; i<=getpid(); i++) {
    len = sizeof buf;
    mib[3] = i;
    if (sysctl(mib, 4, buf, &len, NULL, 0) != -1) {
      struct kinfo_proc *kp = (struct kinfo_proc*)buf;
      printf("%8u  %8u %8u %8u   %s\n",
	     kp->pid, kp->ppid, kp->pgid, kp->sid, kp->name);
    }
  }
  
  return 0;
}

