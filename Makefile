#
#     This file is part of MutekH.
#
#     MutekH is free software; you can redistribute it and/or modify it
#     under the terms of the GNU General Public License as published by
#     the Free Software Foundation; either version 2 of the License, or
#     (at your option) any later version.
#
#     MutekH is distributed in the hope that it will be useful, but
#     WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with MutekH; if not, write to the Free Software Foundation,
#     Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

ifndef BUILD_DIR
BUILD_DIR:=$(shell pwd)
endif
ifdef SRC_DIR
MUTEK_SRC_DIR:=$(SRC_DIR)
else
MUTEK_SRC_DIR:=$(shell pwd)
endif
CURRENT_DIR:=$(shell pwd)
ifndef USER_DIR
USER_DIR:=$(CURRENT_DIR)
endif
CONF=myconfig

export BUILD?=default
BUILD_DIR:=$(realpath $(BUILD_DIR))
MUTEK_SRC_DIR:=$(realpath $(MUTEK_SRC_DIR))
CONF:=$(realpath $(CONF))

export MUTEK_SRC_DIR
export BUILD_DIR
export CONF_DIR
export CURRENT_DIR
export USER_DIR
export MODULES

ifneq ($(VERBOSE),1)
MAKEFLAGS = -s
endif

CONF_DIR:=$(shell echo /tmp/.mutekh_config.$$$$.$${RANDOM})

all: kernel

.PHONY: FORCE

mkmf: config
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/rules_main.mk $@ TARGET_MK=$(TARGET_MK) MAKEFLAGS=$(MAKEFLAGS)

helpconfig listconfig showconfig listallconfig:
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/config.mk $@

# Test for heterogeneous builds
ifeq ($(EACH),)

# not heterogeneous

clean:
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/rules_main.mk $@ CLEANING=1 MAKEFLAGS=$(MAKEFLAGS)
	-rm -f $(CONF_DIR)/.config.* 2>/dev/null

config showpaths kernel cflags objs: FORCE
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/rules_main.mk $@ MAKEFLAGS=$(MAKEFLAGS)

else

# heterogeneous

clean:
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/heterogeneous.mk $@ CLEANING=1 MAKEFLAGS=$(MAKEFLAGS) CONF=$(CONF)
	-rm -f $(CONF_DIR)/.config.* 2>/dev/null

kernel kernel-het: FORCE
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/heterogeneous.mk $@ MAKEFLAGS=$(MAKEFLAGS) CONF=$(CONF)

endif # end heterogeneous

FORCE::
	@true

kernel-postlink:  FORCE
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/rules_main.mk $@ MAKEFLAGS=$(MAKEFLAGS) POST_LDSCRIPT=$(POST_LDSCRIPT) POST_TARGET=$(POST_TARGET)

doc: FORCE
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/doc.mk $@ MAKEFLAGS=$(MAKEFLAGS)

buildtest:
	$(MAKE) -C $(MUTEK_SRC_DIR)/examples/build_tests/ -f test.mk
