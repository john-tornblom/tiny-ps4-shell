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

#pragma once

#include <stdint.h>

uint64_t app_get_attributes(void);
uint64_t app_get_authid(void);
uint64_t app_get_capabilities(void);

int app_set_attributes(uint64_t authid);
int app_set_authid(uint64_t authid);
int app_set_capabilities(uint64_t cas);

int app_jailbreak(void);

