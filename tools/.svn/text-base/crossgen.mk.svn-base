#!/usr/bin/make -f
#
#    Cross compiler generation script
#
#    This file is part of MutekH.
#
#    MutekH is free software; you can redistribute it and/or modify it
#    under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    MutekH is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with MutekH; if not, write to the Free Software Foundation,
#    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#
#    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2009
#


#### LINE 25 IS HERE ####

SERVER=ftp://ftp.gnu.org/gnu

BINUTILS_VER=2.20
BINUTILS_CONF=

GCC_VER=4.4.2
GCC_CONF=--enable-languages=c --disable-libssp

GDB_VER=7.0
GDB_CONF=

TARGET=arm-unknown-elf
PREFIX=/opt/gnu
WORKDIR=/tmp




#### LINE 45 IS HERE ####

# Download URLs
BINUTILS_URL=$(SERVER)/binutils/binutils-$(BINUTILS_VER).tar.bz2
GCC_URL=$(SERVER)/gcc/gcc-$(GCC_VER)/gcc-$(GCC_VER).tar.bz2
GDB_URL=$(SERVER)/gdb/gdb-$(GDB_VER).tar.bz2

# Archives names
BINUTILS_TGZ=$(WORKDIR)/binutils-$(BINUTILS_VER).tar.bz2
GCC_TGZ=$(WORKDIR)/gcc-$(GCC_VER).tar.bz2
GDB_TGZ=$(WORKDIR)/gdb-$(GDB_VER).tar.bz2

# Temporary working directory
BINUTILS_DIR=$(WORKDIR)/binutils-$(BINUTILS_VER)
GCC_DIR=$(WORKDIR)/gcc-$(GCC_VER)
GDB_DIR=$(WORKDIR)/gdb-$(GDB_VER)

# Build dirs
BINUTILS_BDIR=$(WORKDIR)/bu-bld-$(TARGET)-$(BINUTILS_VER)
GCC_BDIR=$(WORKDIR)/gcc-bld-$(TARGET)-$(GCC_VER)
GDB_BDIR=$(WORKDIR)/gdb-bld-$(TARGET)-$(GDB_VER)

# Stamp files
BINUTILS_STAMP=$(WORKDIR)/bu-$(BINUTILS_VER)-stamp
GCC_STAMP=$(WORKDIR)/gcc-$(GCC_VER)-stamp
GDB_STAMP=$(WORKDIR)/gdb-$(GDB_VER)-stamp

help:
	@echo Available targets are: all, gcc, binutils, gdb
	@echo Default configuration is:
	@head $(MAKEFILE_LIST) -n 41 | tail -n 16

all: gcc binutils gdb

.PRECIOUS: $(BINUTILS_TGZ) $(GCC_TGZ) $(GDB_TGZ)
.DELETE_ON_ERROR: $(BINUTILS_STAMP)-wget $(GCC_STAMP)-wget $(GDB_STAMP)-wget \
	$(BINUTILS_STAMP)-$(TARGET)-conf $(BINUTILS_STAMP)-$(TARGET)-build \
	$(GCC_STAMP)-$(TARGET)-conf $(GCC_STAMP)-$(TARGET)-build \
	$(GDB_STAMP)-$(TARGET)-conf $(GDB_STAMP)-$(TARGET)-build

$(BINUTILS_STAMP)-wget:
	touch $@
	wget -c $(BINUTILS_URL) -O $(BINUTILS_TGZ)
$(BINUTILS_TGZ): $(BINUTILS_STAMP)-wget
	touch $@

$(GCC_STAMP)-wget:
	touch $@
	wget -c $(GCC_URL) -O $(GCC_TGZ)
$(GCC_TGZ): $(GCC_STAMP)-wget
	touch $@

$(GDB_STAMP)-wget:
	touch $@
	wget -c $(GDB_URL) -O $(GDB_TGZ)
$(GDB_TGZ): $(GDB_STAMP)-wget
	touch $@

% : %.tar.bz2
	( mkdir -p $@ ; cd $@/.. ; tar xjf $< )
	touch $@


$(BINUTILS_STAMP)-$(TARGET)-conf: $(BINUTILS_DIR)
	mkdir -p $(BINUTILS_BDIR)
	( cd $(BINUTILS_BDIR) ; $(BINUTILS_DIR)/configure --prefix=$(PREFIX) --target=$(TARGET) --disable-checking --disable-werror $(BINUTILS_CONF) ) && touch $@

$(BINUTILS_STAMP)-$(TARGET)-build: $(BINUTILS_STAMP)-$(TARGET)-conf
	make -C $(BINUTILS_BDIR) && touch $@

$(PREFIX)/bin/$(TARGET)-as: $(BINUTILS_STAMP)-$(TARGET)-build
	make -C $(BINUTILS_BDIR) install && touch $@

binutils: $(PREFIX)/bin/$(TARGET)-as



$(GCC_STAMP)-$(TARGET)-conf: $(GCC_DIR) binutils
	mkdir -p $(GCC_BDIR)
	( cd $(GCC_BDIR) ; $(GCC_DIR)/configure --prefix=$(PREFIX) --target=$(TARGET) --disable-checking --disable-werror $(GCC_CONF) ) && touch $@

$(GCC_STAMP)-$(TARGET)-build: $(GCC_STAMP)-$(TARGET)-conf
	make -C $(GCC_BDIR) && touch $@

$(PREFIX)/bin/$(TARGET)-gcc: $(GCC_STAMP)-$(TARGET)-build
	make -C $(GCC_BDIR) install && touch $@

gcc: $(PREFIX)/bin/$(TARGET)-gcc



$(GDB_STAMP)-$(TARGET)-conf: $(GDB_DIR)
	mkdir -p $(GDB_BDIR)
	( cd $(GDB_BDIR) ; $(GDB_DIR)/configure --prefix=$(PREFIX) --target=$(TARGET) --disable-checking --disable-werror $(GDB_CONF) ) && touch $@

$(GDB_STAMP)-$(TARGET)-build: $(GDB_STAMP)-$(TARGET)-conf
	make -C $(GDB_BDIR) && touch $@

$(PREFIX)/bin/$(TARGET)-gdb: $(GDB_STAMP)-$(TARGET)-build
	make -C $(GDB_BDIR) install && touch $@

gdb: $(PREFIX)/bin/$(TARGET)-gdb

