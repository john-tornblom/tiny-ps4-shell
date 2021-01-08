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

// Code inspired by https://github.com/Scene-Collective/ps4-pup-decrypt

#pragma once

#include <stdint.h>


struct auditinfo_addr {
  char useless[184];
};


struct ucred {
  uint32_t useless1;
  uint32_t cr_uid;     // effective user id
  uint32_t cr_ruid;    // real user id
  uint32_t useless2;
  uint32_t useless3;
  uint32_t cr_rgid;    // real group id
  uint32_t useless4;
  void *useless5;
  void *useless6;
  void *cr_prison;     // jail(2)
  void *useless7;
  uint32_t useless8;
  void *useless9[2];
  void *useless10;
  struct auditinfo_addr useless11;
  uint32_t *cr_groups; // groups
  uint32_t useless12;
};


struct filedesc {
  void *useless1[3];
  void *fd_rdir;
  void *fd_jdir;
};


struct proc {
  char useless[64];
  struct ucred *p_ucred;
  struct filedesc *p_fd;
};


struct thread {
  void *useless;
  struct proc *td_proc;
};

