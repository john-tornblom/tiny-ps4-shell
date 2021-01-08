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


#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include "kern_orbis.h"
#include "kern_orbis_types.h"
#include "kern_orbis_offsets.h"


#define SYS_kexec 11

long int syscall(long int num, ...);


unsigned long long int __readmsr(unsigned long __register) {
  unsigned long __edx;
  unsigned long __eax;
  __asm__("rdmsr" : "=d"(__edx), "=a"(__eax) : "c"(__register));
  return (((unsigned long long int)__edx) << 32) | (unsigned long long int)__eax;
}


static unsigned int
kern_get_fw_version(void) {
  return 0x672;
}


static int
kern_get_offsets(struct kern_offset *kern) {
  void *base;
  unsigned char *ptr;

  kern->prison0 = 0;
  kern->root_vnode = 0;
  kern->copyin = 0;
  kern->copyout = 0;

  switch(kern_get_fw_version()) {
  case 0x350:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K350_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K350_PRISON_0];
    kern->root_vnode = (void **)&ptr[K350_ROOTVNODE];
    kern->copyin = (void *)(base + K350_COPYIN);
    kern->copyout = (void *)(base + K350_COPYOUT);
    break;
    
  case 0x355:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K355_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K355_PRISON_0];
    kern->root_vnode = (void **)&ptr[K355_ROOTVNODE];
    kern->copyin = (void *)(base + K355_COPYIN);
    kern->copyout = (void *)(base + K355_COPYOUT);
    break;
    
  case 0x370:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K370_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K370_PRISON_0];
    kern->root_vnode = (void **)&ptr[K370_ROOTVNODE];
    kern->copyin = (void *)(base + K370_COPYIN);
    kern->copyout = (void *)(base + K370_COPYOUT);
    break;
    
  case 0x400:
  case 0x401:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K400_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K400_PRISON_0];
    kern->root_vnode = (void **)&ptr[K400_ROOTVNODE];
    kern->copyin = (void *)(base + K400_COPYIN);
    kern->copyout = (void *)(base + K400_COPYOUT);
    break;
    
  case 0x405:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K405_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K405_PRISON_0];
    kern->root_vnode = (void **)&ptr[K405_ROOTVNODE];
    kern->copyin = (void *)(base + K405_COPYIN);
    kern->copyout = (void *)(base + K405_COPYOUT);
    break;

  case 0x406:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K406_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K406_PRISON_0];
    kern->root_vnode = (void **)&ptr[K406_ROOTVNODE];
    kern->copyin = (void *)(base + K406_COPYIN);
    kern->copyout = (void *)(base + K406_COPYOUT);
    break;

  case 0x407:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K407_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K407_PRISON_0];
    kern->root_vnode = (void **)&ptr[K407_ROOTVNODE];
    kern->copyin = (void *)(base + K407_COPYIN);
    kern->copyout = (void *)(base + K407_COPYOUT);
    break;

  case 0x450:
  case 0x455:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K450_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K450_PRISON_0];
    kern->root_vnode = (void **)&ptr[K450_ROOTVNODE];
    kern->copyin = (void *)(base + K450_COPYIN);
    kern->copyout = (void *)(base + K450_COPYOUT);
    break;

  case 0x470:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K470_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K470_PRISON_0];
    kern->root_vnode = (void **)&ptr[K470_ROOTVNODE];
    kern->copyin = (void *)(base + K470_COPYIN);
    kern->copyout = (void *)(base + K470_COPYOUT);
    break;

  case 0x471:
  case 0x472:
  case 0x473:
  case 0x474:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K471_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K471_PRISON_0];
    kern->root_vnode = (void **)&ptr[K471_ROOTVNODE];
    kern->copyin = (void *)(base + K471_COPYIN);
    kern->copyout = (void *)(base + K471_COPYOUT);
    break;

  case 0x500:
  case 0x501:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K501_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K500_PRISON_0];
    kern->root_vnode = (void **)&ptr[K500_ROOTVNODE];
    kern->copyin = (void *)(base + K500_COPYIN);
    kern->copyout = (void *)(base + K500_COPYOUT);
    break;

  case 0x503:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K503_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K503_PRISON_0];
    kern->root_vnode = (void **)&ptr[K503_ROOTVNODE];
    kern->copyin = (void *)(base + K503_COPYIN);
    kern->copyout = (void *)(base + K503_COPYOUT);
    break;

  case 0x505:
  case 0x507:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K505_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K505_PRISON_0];
    kern->root_vnode = (void **)&ptr[K505_ROOTVNODE];
    kern->copyin = (void *)(base + K505_COPYIN);
    kern->copyout = (void *)(base + K505_COPYOUT);
    break;
    
  case 0x550:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K550_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K550_PRISON_0];
    kern->root_vnode = (void **)&ptr[K550_ROOTVNODE];
    kern->copyin = (void *)(base + K550_COPYIN);
    kern->copyout = (void *)(base + K550_COPYOUT);
    break;

  case 0x553:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K553_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K553_PRISON_0];
    kern->root_vnode = (void **)&ptr[K553_ROOTVNODE];
    kern->copyin = (void *)(base + K553_COPYIN);
    kern->copyout = (void *)(base + K553_COPYOUT);
    break;

  case 0x555:
  case 0x556:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K555_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K555_PRISON_0];
    kern->root_vnode = (void **)&ptr[K555_ROOTVNODE];
    kern->copyin = (void *)(base + K556_COPYIN);
    kern->copyout = (void *)(base + K556_COPYOUT);
    break;

  case 0x600:
  case 0x602:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K600_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K600_PRISON_0];
    kern->root_vnode = (void **)&ptr[K600_ROOTVNODE];
    kern->copyin = (void *)(base + K600_COPYIN);
    kern->copyout = (void *)(base + K600_COPYOUT);
    break;

  case 0x620:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K620_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K620_PRISON_0];
    kern->root_vnode = (void **)&ptr[K620_ROOTVNODE];
    kern->copyin = (void *)(base + K620_COPYIN);
    kern->copyout = (void *)(base + K620_COPYOUT);
    break;

  case 0x650:
  case 0x651:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K650_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K650_PRISON_0];
    kern->root_vnode = (void **)&ptr[K650_ROOTVNODE];
    kern->copyin = (void *)(base + K650_COPYIN);
    kern->copyout = (void *)(base + K650_COPYOUT);
    break;
    
  case 0x670:
  case 0x671:
  case 0x672:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K670_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K670_PRISON_0];
    kern->root_vnode = (void **)&ptr[K670_ROOTVNODE];
    kern->copyin = (void *)(base + K670_COPYIN);
    kern->copyout = (void *)(base + K670_COPYOUT);
    break;

  case 0x700:
  case 0x701:
  case 0x702:
    base = &((unsigned char *)__readmsr(0xC0000082))[-K700_XFAST_SYSCALL];
    ptr = (unsigned char *)base;
    kern->prison0 = (void **)&ptr[K700_PRISON_0];
    kern->root_vnode = (void **)&ptr[K700_ROOTVNODE];
    kern->copyin = (void *)(base + K700_COPYIN);
    kern->copyout = (void *)(base + K700_COPYOUT);
    break;

  default:
    return -1;
  }

  return 0;
}


static int
kexec_jailbreak(struct thread *td) {
  struct ucred* cred = td->td_proc->p_ucred;
  struct filedesc* fd = td->td_proc->p_fd;
  struct kern_offset kern;
  
  kern_get_offsets(&kern);
  if(!kern.prison0 || !kern.root_vnode) {
    return EFAULT;
  }
  
  cred->cr_prison = *(kern.prison0);
  fd->fd_rdir = fd->fd_jdir = *(kern.root_vnode);

  return 0;
}


static int
kexec_get_attributes(struct thread *td, struct kexec_ctx *ctx) {
  void *td_ucred = *(void **)(((char *)td) + 304);
  const uint64_t *attrs = (uint64_t *)(((char *)td_ucred) + 96);
  struct kern_offset kern;

  kern_get_offsets(&kern);
  if(!kern.copyout) {
    return EFAULT;
  }
  
  return kern.copyout(attrs, ctx->uaddr, sizeof(uint64_t));
}


static int
kexec_get_authid(struct thread *td, struct kexec_ctx *ctx) {
  void *td_ucred = *(void **)(((char *)td) + 304);
  const uint64_t *authid = (uint64_t *)(((char *)td_ucred) + 88);
  struct kern_offset kern;

  kern_get_offsets(&kern);
  if(!kern.copyout) {
    return EFAULT;
  }
  
  return kern.copyout(authid, ctx->uaddr, sizeof(uint64_t));
}


static int
kexec_get_capabilities(struct thread *td, struct kexec_ctx *ctx) {
  void *td_ucred = *(void **)(((char *)td) + 304);
  const uint64_t *caps = (uint64_t *)(((char *)td_ucred) + 104);
  struct kern_offset kern;

  kern_get_offsets(&kern);
  if(!kern.copyout) {
    return EFAULT;
  }
  
  return kern.copyout(caps, ctx->uaddr, sizeof(uint64_t));  
}


static int
kexec_set_attributes(struct thread *td, struct kexec_ctx *ctx) {
  void *td_ucred = *(void **)(((char *)td) + 304);
  uint64_t *attrs = (uint64_t *)(((char *)td_ucred) + 96);
  struct kern_offset kern;
  
  kern_get_offsets(&kern);
  if(!kern.copyin) {
    return EFAULT;
  }
  
  return kern.copyin(ctx->uaddr, attrs, sizeof(uint64_t));
}


static int
kexec_set_authid(struct thread *td, struct kexec_ctx *ctx) {
  void *td_ucred = *(void **)(((char *)td) + 304);
  uint64_t *authid = (uint64_t *)(((char *)td_ucred) + 88);
  struct kern_offset kern;

  kern_get_offsets(&kern);
  if(!kern.copyin) {
    return EFAULT;
  }

  return kern.copyin(ctx->uaddr, authid, sizeof(uint64_t));
}


static int
kexec_set_capabilities(struct thread *td, struct kexec_ctx *ctx) {
  void *td_ucred = *(void **)(((char *)td) + 304);
  uint64_t *caps = (uint64_t *)(((char *)td_ucred) + 104);
  struct kern_offset kern;
  
  kern_get_offsets(&kern);
  if(!kern.copyin) {
    return EFAULT;
  }
  
  return kern.copyin(ctx->uaddr, caps, sizeof(uint64_t));
}


static int
app_get(void *fn, void *ptr, size_t len) {
  int rc = -1;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  void *uaddr = mmap(0, len, prot, flags, -1, 0);
  
  if(!syscall(SYS_kexec, fn, uaddr)) {
    memcpy(ptr, uaddr, len);
    rc = 0;
  }

  munmap(uaddr, len);
  
  return rc;
}


static int
app_set(void *fn, void *ptr, size_t len) {
  int rc = -1;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  void *uaddr = mmap(0, len, prot, flags, -1, 0);

  memcpy(uaddr, ptr, len);
  if(!syscall(SYS_kexec, fn, uaddr)) {
    rc = 0;
  }

  munmap(uaddr, len);
  
  return rc;
}


uint64_t
app_get_authid(void) {
  uint64_t authid = 0;

  if(app_get(&kexec_get_authid, &authid, sizeof(authid))) {
    return 0;
  }
  
  return authid;
}


uint64_t
app_get_capabilities(void) {
  uint64_t caps = 0;
  
  if(app_get(&kexec_get_capabilities, &caps, sizeof(caps))) {
    return 0;
  }
  
  return caps;
}


uint64_t
app_get_attributes(void) {
  uint64_t attrs = 0;
  
  if(app_get(&kexec_get_attributes, &attrs, sizeof(attrs))) {
    return 0;
  }
  
  return attrs;
}


int
app_set_attributes(uint64_t val) {
  uint64_t attrs = val;
  
  return app_set(&kexec_set_attributes, &attrs, sizeof(attrs));
}


int
app_set_authid(uint64_t val) {
  uint64_t authid = val;
  
  return app_set(&kexec_set_authid, &authid, sizeof(authid));
}


int
app_set_capabilities(uint64_t val) {
  uint64_t caps = val;
  
  return app_set(&kexec_set_capabilities, &caps, sizeof(caps));
}


int
app_jailbreak(void) {
  if(syscall(SYS_kexec, kexec_jailbreak)) {
    return -1;
  }

  return 0;
}

