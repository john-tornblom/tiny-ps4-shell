
// Code derived from https://github.com/AlexAltea/orbital

#include <assert.h>
#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "_common.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define MAP_SELF 0x80000
#define PT_NID 0x61000000


typedef struct self_entry_t {
  uint32_t props;
  uint32_t reserved;
  uint64_t offset;
  uint64_t filesz;
  uint64_t memsz;
} self_entry_t;

typedef struct self_header_t {
  uint32_t magic;
  uint8_t version;
  uint8_t mode;
  uint8_t endian;
  uint8_t attr;
  uint32_t key_type;
  uint16_t header_size;
  uint16_t meta_size;
  uint64_t file_size;
  uint16_t num_entries;
  uint16_t flags;
  uint32_t reserved;
  self_entry_t entries[0];
} self_header_t;



typedef struct self_context_t {
  uint32_t format;
  uint32_t elf_auth_type;
  uint32_t total_header_size;
  uint32_t unk_0C;
  void *segment;
  uint32_t unk_18;
  uint32_t ctx_id;
  uint64_t svc_id;
  uint64_t unk_28;
  uint32_t buf_id;
  uint32_t unk_34;
  struct self_header_t *header;
  uint8_t mtx_struct[0x20];
} self_context_t;


struct self_auth_info_t {
  uint8_t buf[0x88];
};


typedef struct self_t {
  int fd;
  char *file_path;
  size_t file_size;
  size_t entries_size;
  size_t data_offset;
  size_t data_size;
  /* contents */
  struct self_header_t header;
  struct self_entry_t *entries;
  uint8_t *data;
  /* kernel */
  struct self_auth_info_t auth_info;
  struct self_context_t *ctx;
  int auth_ctx_id;
  int ctx_id;
  int svc_id;
  int verified;
} self_t;


static int
valid_elf_magic(const uint8_t *magic) {
  return ((magic[0] == 0x7F) &&
	  (magic[1] == 0x45) &&
	  (magic[2] == 0x4C) &&
	  (magic[3] == 0x46));
}


static uint8_t*
self_decrypt(uint8_t *self_data, size_t self_size, int self_fd, size_t *elf_sizep) {
  uint8_t *elf_data;
  size_t ehdr_off;
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  uint8_t *elf_base;
  uint8_t *elf_segment;
  size_t elf_size;
  size_t i, j, off;

  // Determine actual format
  ehdr_off = 0;
  ehdr = (void*)&self_data[ehdr_off];
  if (valid_elf_magic(ehdr->e_ident)) {
    elf_data = malloc(self_size);
    memcpy(elf_data, self_data, self_size);
    *elf_sizep = self_size;
    return elf_data;
  } else {
    self_header_t *self_hdr = (void*)self_data;
    ehdr_off += sizeof(self_header_t);
    ehdr_off += sizeof(self_entry_t) * self_hdr->num_entries;
    ehdr = (void*)&self_data[ehdr_off];
    elf_base = (void*)ehdr;
  }

  // Parse EHDR
  if (!valid_elf_magic(ehdr->e_ident)) {
    fprintf(stderr, "Invalid file format\n");
    return NULL;
  }
  ehdr->e_shoff = 0;
  ehdr->e_shnum = 0;
  elf_size = (ehdr->e_phoff + (ehdr->e_phnum * ehdr->e_phentsize) +
	      (ehdr->e_shnum * ehdr->e_shentsize));

  // Parse PHDRs
  phdr = (void*)&elf_base[ehdr->e_phoff];
  for (i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD ||
	phdr[i].p_type == PT_NID  ||
	phdr[i].p_type == PT_DYNAMIC) {
      elf_size = MAX(elf_size, phdr[i].p_offset + phdr[i].p_filesz);
    }
  }

  *elf_sizep = elf_size;
  elf_data = malloc(elf_size);

  // Copy EHDR
  for (i = 0; i < ehdr->e_ehsize; i++) {
    elf_data[i] = elf_base[i];
  }
  
  // Copy PHDRs and segments
  for (i = 0; i < ehdr->e_phnum; i++) {
    for (j = 0; j < ehdr->e_phentsize; j++) {
      off = ehdr->e_phoff + (i * ehdr->e_phentsize) + j;
      elf_data[off] = elf_base[off];
    }
    if (phdr[i].p_type == PT_LOAD || phdr[i].p_type == PT_NID) {
      lseek(self_fd, 0, SEEK_SET);
      elf_segment = mmap(NULL, phdr[i].p_filesz, PROT_READ, MAP_SHARED | MAP_SELF, self_fd, i << 32);
      for (j = 0; j < phdr[i].p_filesz; j++) {
	elf_data[phdr[i].p_offset + j] = elf_segment[j];
      }
      munmap(elf_segment, phdr[i].p_filesz);
    }
  }
  return elf_data;
}



static uint8_t*
self_decrypt_fd(int fd, size_t *elf_sizep) {
  uint8_t *elf_data = NULL;
  uint8_t *self_data = NULL;
  size_t self_size;
  size_t off;

  off = lseek(fd, 0, SEEK_END);
  assert(off >= 0);
  self_size = off;
  
  off = lseek(fd, 0, SEEK_SET);
  assert(off >= 0);

  self_data = malloc(self_size);
  assert(self_data);
  
  off = read(fd, self_data, self_size);
  assert(off == self_size);
  
  elf_data = self_decrypt(self_data, self_size, fd, elf_sizep);

  free(self_data);
  return elf_data;
}


static uint8_t*
self_decrypt_file(const char *file, size_t *elf_sizep) {
  uint8_t *elf_data = NULL;
  int fd;

  fd = open(file, O_RDONLY, 0);
  assert(fd >= 0);
  
  elf_data = self_decrypt_fd(fd, elf_sizep);
  close(fd);

  return elf_data;
}


int
main_self2elf(int argc, char **argv) {  
  if(argc <= 1) {
    fprintf(stderr, "usage: %s <SELF> [ELF]\n", argv[0]);
    return -1;
  }

  char *path = abspath(argv[1]);
  size_t elf_size = 0;
  uint8_t *elf_data = self_decrypt_file(path, &elf_size);
  
  free(path);

  
  FILE *fp = stdout;
  if(argc > 2) {
    path = abspath(argv[2]);
    if(!(fp = fopen(path, "wb"))) {
      perror(argv[2]);
    }
  }

  fwrite(elf_data, sizeof(uint8_t), elf_size, fp);
  
  return 0;
}
