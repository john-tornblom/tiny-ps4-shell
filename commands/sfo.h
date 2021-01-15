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

// Code derived from https://github.com/TheOfficialFloW/VitaShell

#pragma once

#include <stdint.h>

typedef struct sfo_header {
  uint32_t magic;
  uint32_t version;
  uint32_t keys_offset;
  uint32_t data_offset;
  uint32_t count;
} __attribute__((packed)) sfo_header_t;


typedef struct sfo_entry {
  uint16_t key_offset;
  uint8_t  alignment;
  uint8_t  type;
  uint32_t val_length;
  uint32_t val_size;
  uint32_t val_offset;
} __attribute__((packed)) sfo_entry_t;


#define TYPE_BIN 0
#define TYPE_STR 2
#define TYPE_INT 4

#define MAGIC 0x46535000

