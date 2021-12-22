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

#define K350_XFAST_SYSCALL         0x003A1AD0
#define K355_XFAST_SYSCALL         0x003A1F10
#define K370_XFAST_SYSCALL         0x003A2000
#define K400_XFAST_SYSCALL         0x0030EA00
#define K401_XFAST_SYSCALL         0x0030EA00
#define K405_XFAST_SYSCALL         0x0030EB30
#define K406_XFAST_SYSCALL         0x0030EB40
#define K407_XFAST_SYSCALL         0x0030EB40
#define K450_XFAST_SYSCALL         0x003095D0
#define K455_XFAST_SYSCALL         0x003095D0
#define K470_XFAST_SYSCALL         0x0030B840
#define K471_XFAST_SYSCALL         0x0030B7D0
#define K472_XFAST_SYSCALL         0x0030B7D0
#define K473_XFAST_SYSCALL         0x0030B7D0
#define K474_XFAST_SYSCALL         0x0030B7D0
#define K500_XFAST_SYSCALL         0x000001C0
#define K501_XFAST_SYSCALL         0x000001C0
#define K503_XFAST_SYSCALL         0x000001C0
#define K505_XFAST_SYSCALL         0x000001C0
#define K507_XFAST_SYSCALL         0x000001C0
#define K550_XFAST_SYSCALL         0x000001C0
#define K553_XFAST_SYSCALL         0x000001C0
#define K555_XFAST_SYSCALL         0x000001C0
#define K556_XFAST_SYSCALL         0x000001C0
#define K600_XFAST_SYSCALL         0x000001C0
#define K602_XFAST_SYSCALL         0x000001C0
#define K620_XFAST_SYSCALL         0x000001C0
#define K650_XFAST_SYSCALL         0x000001C0
#define K651_XFAST_SYSCALL         0x000001C0
#define K670_XFAST_SYSCALL         0x000001C0
#define K671_XFAST_SYSCALL         0x000001C0
#define K672_XFAST_SYSCALL         0x000001C0
#define K700_XFAST_SYSCALL         0x000001C0
#define K750_XFAST_SYSCALL         0x000001C0
#define K751_XFAST_SYSCALL         0x000001C0
#define K755_XFAST_SYSCALL         0x000001C0
#define K900_XFAST_SYSCALL         0x000001C0

#define K350_PRISON_0              0x00EF5A00
#define K355_PRISON_0              0x00EF5A00
#define K370_PRISON_0              0x00EFEF10
#define K400_PRISON_0              0x00F26010
#define K401_PRISON_0              0x00F26010
#define K405_PRISON_0              0x00F26010
#define K406_PRISON_0              0x00F26010
#define K407_PRISON_0              0x00F2A010
#define K450_PRISON_0              0x010399B0
#define K455_PRISON_0              0x010399B0
#define K470_PRISON_0              0x01042AB0
#define K471_PRISON_0              0x01042AB0
#define K472_PRISON_0              0x01042AB0
#define K473_PRISON_0              0x01042AB0
#define K474_PRISON_0              0x01042AB0
#define K500_PRISON_0              0x010986A0
#define K501_PRISON_0              0x010986A0
#define K503_PRISON_0              0x010986A0
#define K505_PRISON_0              0x010986A0
#define K507_PRISON_0              0x010986A0
#define K550_PRISON_0              0x01134180
#define K553_PRISON_0              0x01134180
#define K555_PRISON_0              0x01139180
#define K556_PRISON_0              0x01139180
#define K600_PRISON_0              0x01139458
#define K602_PRISON_0              0x01139458
#define K620_PRISON_0              0x0113D458
#define K650_PRISON_0              0x0113D4F8
#define K651_PRISON_0              0x0113D4F8
#define K670_PRISON_0              0x0113E518
#define K671_PRISON_0              0x0113E518
#define K672_PRISON_0              0x0113E518
#define K700_PRISON_0              0x0113E398
#define K750_PRISON_0              0x0113B728
#define K751_PRISON_0              0x0113B728
#define K755_PRISON_0              0x0113B728
#define K900_PRISON_0              0x0111F870

#define K350_ROOTVNODE             0x01963000
#define K355_ROOTVNODE             0x01963040
#define K370_ROOTVNODE             0x0196F040
#define K400_ROOTVNODE             0x0206D250
#define K401_ROOTVNODE             0x0206D250
#define K405_ROOTVNODE             0x0206D250
#define K406_ROOTVNODE             0x0206D250
#define K407_ROOTVNODE             0x02071250
#define K450_ROOTVNODE             0x021AFA30
#define K455_ROOTVNODE             0x021AFA30
#define K470_ROOTVNODE             0x021B89E0
#define K471_ROOTVNODE             0x021B89E0
#define K472_ROOTVNODE             0x021B89E0
#define K473_ROOTVNODE             0x021B89E0
#define K474_ROOTVNODE             0x021B89E0
#define K500_ROOTVNODE             0x022C19F0
#define K501_ROOTVNODE             0x022C19F0
#define K503_ROOTVNODE             0x022C1A70
#define K505_ROOTVNODE             0x022C1A70
#define K507_ROOTVNODE             0x022C1A70
#define K550_ROOTVNODE             0x022EF570
#define K553_ROOTVNODE             0x022EF570
#define K555_ROOTVNODE             0x022F3570
#define K556_ROOTVNODE             0x022F3570
#define K600_ROOTVNODE             0x021BFAC0
#define K602_ROOTVNODE             0x021BFAC0
#define K620_ROOTVNODE             0x021C3AC0
#define K650_ROOTVNODE             0x02300320
#define K651_ROOTVNODE             0x02300320
#define K670_ROOTVNODE             0x02300320
#define K671_ROOTVNODE             0x02300320
#define K672_ROOTVNODE             0x02300320
#define K700_ROOTVNODE             0x022C5750
#define K750_ROOTVNODE             0x01B463E0
#define K751_ROOTVNODE             0x01B463E0
#define K755_ROOTVNODE             0x01B463E0
#define K900_ROOTVNODE             0x021EFF20

#define K350_COPYIN                0x003B92A0
#define K355_COPYIN                0x003B96E0
#define K370_COPYIN                0x003B97D0
#define K400_COPYIN                0x00286CC0
#define K401_COPYIN                0x00286CC0
#define K405_COPYIN                0x00286DF0
#define K406_COPYIN                0x00286DF0
#define K407_COPYIN                0x00286DF0
#define K450_COPYIN                0x0014A890
#define K455_COPYIN                0x0014A890
#define K470_COPYIN                0x00149F20
#define K471_COPYIN                0x00149F20
#define K472_COPYIN                0x00149F20
#define K473_COPYIN                0x00149F20
#define K474_COPYIN                0x00149F20
#define K500_COPYIN                0x001EA600
#define K501_COPYIN                0x001EA600
#define K503_COPYIN                0x001EA710
#define K505_COPYIN                0x001EA710
#define K507_COPYIN                0x001EA710
#define K550_COPYIN                0x00405BB0
#define K553_COPYIN                0x00405AB0
#define K555_COPYIN                0x00405E70
#define K556_COPYIN                0x00405E70
#define K600_COPYIN                0x001148F0
#define K602_COPYIN                0x001148F0
#define K620_COPYIN                0x001148F0
#define K650_COPYIN                0x003C13F0
#define K651_COPYIN                0x003C13F0
#define K670_COPYIN                0x003C17A0
#define K671_COPYIN                0x003C17A0
#define K672_COPYIN                0x003C17A0
#define K700_COPYIN                0x0002F230
#define K750_COPYIN                0x0028F9F0
#define K751_COPYIN                0x0028F9F0
#define K755_COPYIN                0x0028F9F0
#define K900_COPYIN                0x002716A0

#define K350_COPYOUT               0x003B9220
#define K355_COPYOUT               0x003B9660
#define K370_COPYOUT               0x003B9750
#define K400_COPYOUT               0x00286C40
#define K401_COPYOUT               0x00286C40
#define K405_COPYOUT               0x00286D70
#define K406_COPYOUT               0x00286D70
#define K407_COPYOUT               0x00286D70
#define K450_COPYOUT               0x0014A7B0
#define K455_COPYOUT               0x0014A7B0
#define K470_COPYOUT               0x00149E40
#define K471_COPYOUT               0x00149E40
#define K472_COPYOUT               0x00149E40
#define K473_COPYOUT               0x00149E40
#define K474_COPYOUT               0x00149E40
#define K500_COPYOUT               0x001EA520
#define K501_COPYOUT               0x001EA520
#define K503_COPYOUT               0x001EA630
#define K505_COPYOUT               0x001EA630
#define K507_COPYOUT               0x001EA630
#define K550_COPYOUT               0x00405AC0
#define K553_COPYOUT               0x004059C0
#define K555_COPYOUT               0x00405D80
#define K556_COPYOUT               0x00405D80
#define K600_COPYOUT               0x00114800
#define K602_COPYOUT               0x00114800
#define K620_COPYOUT               0x00114800
#define K650_COPYOUT               0x003C1300
#define K651_COPYOUT               0x003C1300
#define K670_COPYOUT               0x003C16B0
#define K671_COPYOUT               0x003C16B0
#define K672_COPYOUT               0x003C16B0
#define K700_COPYOUT               0x0002F140
#define K750_COPYOUT               0x0028F900
#define K751_COPYOUT               0x0028F900
#define K755_COPYOUT               0x0028F900
#define K900_COPYOUT               0x002715B0



struct kexec_ctx {
  void *syscall_handler;
  unsigned int sw_ver;
  void *uaddr;
};


typedef int (copy_t)(const void *from, void *to, unsigned int len);


struct kern_offset {
  void  **prison0;
  void  **root_vnode;
  copy_t *copyin;
  copy_t *copyout;
};

