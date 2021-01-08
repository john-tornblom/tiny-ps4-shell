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
#include <sys/types.h>
#include <unistd.h>


/**
 * 
 **/
int
main_id(int argc, char **argv) {  
  uid_t egid = getegid();
  gid_t rgid = getgid();
  uid_t euid = geteuid();
  gid_t ruid = getuid();

  printf("uid=%u gid=%u", ruid, rgid);

  if(euid != ruid) {
    printf(" euid=%u", euid);
  }
  if (egid != rgid) {
    printf(" euid=%u", egid);
  }

  printf("\n");
  
  return 0;
}
