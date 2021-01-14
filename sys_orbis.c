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
#include <sys/wait.h>
#include <orbis/SysUtil.h>

#include "kern_orbis.h"


/**
 * Generate a UI notification.
 **/
void
sys_notify(const char *fmt, ...) {
  va_list args;
  char str[1024];
    
  va_start(args, fmt);
  vsnprintf(str, sizeof(str), fmt, args);
  va_end(args);

  sceSysUtilSendSystemNotificationWithText(222, str);
}


/**
 * The fork syscall needs additional application attributes.
 **/
pid_t
sys_fork(void) {
  uint64_t attrs = app_get_attributes();

  app_set_attributes(attrs | (1ULL << 62));

  pid_t pid = fork();
  
  app_set_attributes(attrs);
  
  return pid;
}


pid_t
sys_waitpid(pid_t pid, int *wstatus, int options) {
  uint64_t attrs = app_get_attributes();

  app_set_attributes(attrs | (1ULL << 62));

  pid = waitpid(pid, wstatus, options);
  
  app_set_attributes(attrs);
  
  return pid;
}


/**
 * The dup syscall needs aditional application capabilities.
 **/
int sys_dup(int oldfd) {
  uint64_t caps = app_get_capabilities();
  
  app_set_capabilities(caps | (1ULL << 62));
  
  int res = dup(oldfd);

  app_set_capabilities(caps);
  
  return res;
}


/**
 * The dup2() function needs aditional application capabilities.
 **/
int
sys_dup2(int oldfd, int newfd) {
  uint64_t caps = app_get_capabilities();
  
  app_set_capabilities(caps | (1ULL << 62));
  
  int res = dup2(oldfd, newfd);

  app_set_capabilities(caps);
  
  return res;
}


/**
 * The pipe syscall needs aditional application capabilities.
 **/
int
sys_pipe(int pipefd[2]) {
  uint64_t caps = app_get_capabilities();
  
  app_set_capabilities(caps | (1ULL << 62));
  
  int res = pipe(pipefd);

  app_set_capabilities(caps);
  
  return res;
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


/**
 * Create a new session so we are not attached to PS4 system services.
 * This ensures kill -1 does not terminate vital services wehn acting as uid0.
 * Also, obtain our process group id so we can signal TERM to child processes
 * when OrbisOS sends STOP.
 **/
void
sys_init(void) {
  uint64_t attrs = app_get_attributes();
  
  app_set_attributes(attrs | (1ULL << 62));
  if(setsid() < 0) {
    sys_notify("setsid: %s", strerror(errno));
  }
  app_set_attributes(attrs);
  
  pgid = getpgrp();
  signal(17, on_SIGSTOP);
}

