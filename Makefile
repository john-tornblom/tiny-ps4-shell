#   Copyright (C) 2021 John Törnblom
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING. If not see
# <http://www.gnu.org/licenses/>.

TITLE      := Telnet Shell
VERSION    := 1.00
TITLE_ID   := BREW00009
CONTENT_ID := IV0000-$(TITLE_ID)_00-TELNETSHELL00000

INSTALL_LIBS := -lkernel -lc -lSceLibcInternal -lSceSysUtil
DAEMON_LIBS  := -lkernel -lc # daemon will not start when linked to sce libs

PKG_FILES := daemon.bin daemon.sfo \
	     icon0.png install.bin install.sfo \
	     libc.prx libSceFios2.prx


COMMANDS := $(wildcard commands/*.c)
COMMANDS := $(COMMANDS:.c=.o)


ifndef OO_PS4_TOOLCHAIN
    $(error OO_PS4_TOOLCHAIN is undefined)
endif

CC := clang-9
LD := ld.lld-9

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PATH := $(PATH):$(OO_PS4_TOOLCHAIN)/bin/linux
endif
ifeq ($(UNAME_S),Darwin)
    PATH := $(PATH):$(OO_PS4_TOOLCHAIN)/bin/macos
endif


CFLAGS := -target x86_64-scei-ps4-elf -funwind-tables \
          -fuse-init-array -isysroot $(OO_PS4_TOOLCHAIN) \
	  -isystem $(OO_PS4_TOOLCHAIN)/include -I. -Wall \
	  -DTITLE_ID='"$(TITLE_ID)"'

all: $(CONTENT_ID).pkg


$(CONTENT_ID).pkg: install.gp4 $(PKG_FILES)
	PkgTool.Core pkg_build $< .


install.sfo:
	PkgTool.Core sfo_new $@
	PkgTool.Core sfo_setentry $@ APP_TYPE --type Integer --maxsize 4 --value 1
	PkgTool.Core sfo_setentry $@ APP_VER --type Utf8 --maxsize 8 --value '$(VERSION)'
	PkgTool.Core sfo_setentry $@ ATTRIBUTE --type Integer --maxsize 4 --value 0
	PkgTool.Core sfo_setentry $@ CATEGORY --type Utf8 --maxsize 4 --value 'gd'
	PkgTool.Core sfo_setentry $@ CONTENT_ID --type Utf8 --maxsize 48 --value '$(CONTENT_ID)'
	PkgTool.Core sfo_setentry $@ DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
	PkgTool.Core sfo_setentry $@ SYSTEM_VER --type Integer --maxsize 4 --value 0
	PkgTool.Core sfo_setentry $@ TITLE --type Utf8 --maxsize 128 --value '$(TITLE)'
	PkgTool.Core sfo_setentry $@ TITLE_ID --type Utf8 --maxsize 12 --value '$(TITLE_ID)'
	PkgTool.Core sfo_setentry $@ VERSION --type Utf8 --maxsize 8 --value '$(VERSION)'


daemon.sfo:
	PkgTool.Core sfo_new $@
	PkgTool.Core sfo_setentry $@ APP_TYPE --type Integer --maxsize 4 --value 0
	PkgTool.Core sfo_setentry $@ APP_VER --type Utf8 --maxsize 8 --value ''
	PkgTool.Core sfo_setentry $@ ATTRIBUTE --type Integer --maxsize 4 --value 0
	PkgTool.Core sfo_setentry $@ CATEGORY --type Utf8 --maxsize 4 --value 'gdd'
	PkgTool.Core sfo_setentry $@ CONTENT_ID --type Utf8 --maxsize 48 --value ''
	PkgTool.Core sfo_setentry $@ DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
	PkgTool.Core sfo_setentry $@ FORMAT --type Utf8 --maxsize 4 --value 'ngh'
	PkgTool.Core sfo_setentry $@ TITLE --type Utf8 --maxsize 128 --value '$(TITLE)'
	PkgTool.Core sfo_setentry $@ TITLE_ID --type Utf8 --maxsize 12 --value '$(TITLE_ID)'
	PkgTool.Core sfo_setentry $@ VERSION --type Utf8 --maxsize 8 --value '$(VERSION)'


icon0.png:
	convert -size 512x512 -gravity center label:'$(TITLE)' $@


%.bin: %.elf
	create-fself -in=$< -eboot $@


install.elf: main_install.o kern_orbis.o
	$(LD) -o $@ -m elf_x86_64 -pie --eh-frame-hdr \
	       --script $(OO_PS4_TOOLCHAIN)/link.x \
               $(OO_PS4_TOOLCHAIN)/lib/crt1.o \
	       $(OO_PS4_TOOLCHAIN)/lib/crti.o \
	       -L$(OO_PS4_TOOLCHAIN)/lib \
	       $^ $(INSTALL_LIBS) \
	       $(OO_PS4_TOOLCHAIN)/lib/crtn.o


daemon.elf: $(COMMANDS) main_server.o kern_orbis.o shell.o sys_orbis.o
	$(LD) -o $@ -m elf_x86_64 -pie --eh-frame-hdr \
	       --script $(OO_PS4_TOOLCHAIN)/link.x \
               $(OO_PS4_TOOLCHAIN)/lib/crt1.o \
	       $(OO_PS4_TOOLCHAIN)/lib/crti.o \
	       -L$(OO_PS4_TOOLCHAIN)/lib \
	       $^ $(DAEMON_LIBS) \
	       $(OO_PS4_TOOLCHAIN)/lib/crtn.o


%.prx:
	cp $(OO_PS4_TOOLCHAIN)/src/modules/$(@F) $@


.INTERMEDIATE: install.sfo icon0.png daemon.sfo libc.prx libSceFios2.prx

clean:
	rm -f install.bin install.elf install.sfo install.png \
	      daemon.bin daemon.elf daemon.sfo \
              $(CONTENT_ID).pkg $(COMMANDS) *.o


kill_daemon:
	-netcat $(PS4_HOST) 2323 < scripts/kill-daemon.nc > /dev/null


deploy: daemon.bin kill_daemon
	curl -T daemon.bin ftp://$(PS4_HOST):1337/system/vsh/app/$(TITLE_ID)/eboot.bin

