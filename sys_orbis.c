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
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <orbis/libkernel.h>

#include "kern_orbis.h"
#include "sys.h"


/**
 * Generate a UI notification.
 **/
void
sys_notify(const char *fmt, ...) {
  OrbisNotificationRequest req;
  va_list args;

  bzero(&req, sizeof req);
  va_start(args, fmt);
  vsnprintf(req.message, sizeof req.message, fmt, args);
  va_end(args);

  sceKernelSendNotificationRequest(0, &req, sizeof req, 0);
}


/**
 * The PS4 port of musl does not use the SYS_getcwd syscall
 * correctly, accommodate for that here.
 **/
char*
sys_getcwd(char *buf, size_t size) {
#define SYS_getcwd 326
  
  if(orbis_syscall(SYS_getcwd, buf, size)) {
    return NULL;
  }
  return buf;
}



/**
 * The PS4 port of musl does not use the SYS_fork syscall
 * correctly, accommodate for that here.
 **/
pid_t
sys_fork(void) {
#define SYS_fork 2
  return orbis_syscall(SYS_fork);
}


void
sys_setprocname(const char* name) {
  sceKernelSetProcessName(name);
}


/**
 * Avoid recurive signals.
 **/
static volatile sig_atomic_t termination_in_progress = 0;


/**
 * Process group for all forked processes.
 **/
static pid_t pgid = 0;


/**
 * OrbisOS sends a STOP signal (17) to the parent process when the user stops
 * an application. Upon such events, politely ask child processes to terminate
 * by signaling TERM (15) to the entire process group.
 **/
static void on_SIGSTOP(int sig) {
  if(termination_in_progress) {
    raise(sig);
  }

  killpg(pgid, SIGTERM);
  termination_in_progress = 1;
  exit(0);
}


#define MAX_MESSAGE_SIZE    0x2000
#define MAX_STACK_FRAMES    32


/**
 * A callframe captures the stack and program pointer of a
 * frame in the call path.
 **/
typedef struct  {
  void *sp;
  void *pc;
} callframe_t;


/**
 * Log a backtrace to /dev/klog
 **/
static void
klog_backtrace(const char* reason) {
  char addr2line[MAX_STACK_FRAMES * 20];
  callframe_t frames[MAX_STACK_FRAMES];
  OrbisKernelVirtualQueryInfo info;
  char buf[MAX_MESSAGE_SIZE + 3];
  unsigned int nb_frames = 0;
  char temp[80];

  memset(addr2line, 0, sizeof addr2line);
  memset(frames, 0, sizeof frames);
  memset(buf, 0, sizeof buf);

  snprintf(buf, sizeof buf, "<118>[Crashlog]: %s\n", reason);
  
  strncat(buf, "<118>[Crashlog]: Backtrace:\n", MAX_MESSAGE_SIZE);
  sceKernelBacktraceSelf(frames, sizeof frames, &nb_frames, 0);
  for(unsigned int i=0; i<nb_frames; i++) {
    memset(&info, 0, sizeof info);
    sceKernelVirtualQuery(frames[i].pc, 0, &info, sizeof info);

    snprintf(temp, sizeof temp,
	     "<118>[Crashlog]:   #%02d %32s: 0x%lx\n",
	     i + 1, info.name, frames[i].pc - info.unk01 - 1);
    strncat(buf, temp, MAX_MESSAGE_SIZE);
    
    snprintf(temp, sizeof temp,
	     "0x%lx ", frames[i].pc - info.unk01 - 1);
    strncat(addr2line, temp, sizeof addr2line - 1);
  }

  strncat(buf, "<118>[Crashlog]: addr2line: ", MAX_MESSAGE_SIZE);
  strncat(buf, addr2line, MAX_MESSAGE_SIZE);
  strncat(buf, "\n", MAX_MESSAGE_SIZE);

  buf[MAX_MESSAGE_SIZE+1] = '\n';
  buf[MAX_MESSAGE_SIZE+2] = '\0';
  
  sceKernelDebugOutText(0, buf);
}


/**
 * Log fatal signals to kernel log.
 **/
static void
on_fatal_signal(int sig) {
  char reason[64];

  sprintf(reason, "Received the fatal POSIX signal %d", sig);
  klog_backtrace(reason);
  _exit(1);
}


/**
 * Create a new session so we are not attached to PS4 system services.
 * This ensures kill -1 does not terminate vital services when acting as uid 0.
 * Also, obtain our process group id so we can signal TERM to child processes
 * when OrbisOS sends STOP.
 **/
void
sys_init(void) {
  uint64_t attrs = app_get_attributes();
  uint64_t caps = app_get_capabilities();

  app_set_attributes(attrs | (1ULL << 62));
  app_set_capabilities(caps | (1ULL << 62));

  setsid();
  pgid = getpgrp();

  signal(5, on_fatal_signal);  // SIGTRAP
  signal(6, on_fatal_signal);  // SIGABRT
  signal(8, on_fatal_signal);  // SIGFPE
  signal(10, on_fatal_signal); // SIGBUS
  signal(11, on_fatal_signal); // SIGSEGV
  signal(17, on_SIGSTOP);
}

