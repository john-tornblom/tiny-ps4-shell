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


typedef struct notify_request {
  char useless1[45];
  char message[3075];
} notify_request_t;


/**
 * Generate a UI notification.
 **/
void
sys_notify(const char *fmt, ...) {
  notify_request_t req;
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
  
  if(syscall(SYS_getcwd, buf, size)) {
    return NULL;
  }
  return buf;
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
  signal(17, on_SIGSTOP);
}

