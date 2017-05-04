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

-include $(arch_SRC_DIR)/config.mk
-include $(cpu_SRC_DIR)/config.mk

CC=$(CCACHE) $(CPUTOOLS)gcc
CXX=$(CCACHE) $(CPUTOOLS)g++
HOSTCC=gcc
DEPCC?=$(CC)
CPP=$(CPUTOOLS)cpp
LD=$(CPUTOOLS)ld
AR=$(CPUTOOLS)ar
AS=$(CPUTOOLS)as
OBJCOPY=$(CPUTOOLS)objcopy
OBJDUMP=$(CPUTOOLS)objdump

CFLAGS=	-nostdlib -fno-builtin -Wall -O$(CONFIG_COMPILE_OPTIMIZE)
DTC=dtc

ifeq ($(CONFIG_OPENMP), defined)
CFLAGS += -fopenmp
endif

ifeq ($(CONFIG_HET_BUILD), defined)
CFLAGS += -fno-section-anchors -ffunction-sections -fdata-sections
endif

ifeq ($(CONFIG_COMPILE_SAVETEMPS), defined)
CFLAGS += -save-temps
endif

ifeq ($(CONFIG_COMPILE_DEBUG), defined)
CFLAGS += -ggdb
endif

ifeq ($(CONFIG_COMPILE_FRAMEPTR), undefined)
CFLAGS += -fomit-frame-pointer
endif

ifeq ($(CONFIG_COMPILE_COLLECT), defined)
CFLAGS += -ffunction-sections -fdata-sections
LINK_LDFLAGS += --gc-sections
endif

ifeq ($(CONFIG_COMPILE_SOFTFLOAT), defined)
CFLAGS += -msoft-float
endif

ifeq ($(CONFIG_COMPILE_INSTRUMENT), defined)
CFLAGS += -finstrument-functions
endif

INCS=-nostdinc -D__MUTEK__ \
	-I$(MUTEK_SRC_DIR)/include \
	$(foreach mod,$(MODULE_NAMES),-I$($(mod)_SRC_DIR)/include) \
	$(foreach mod,$(MODULE_NAMES),-I$($(mod)_OBJ_DIR)/include) \
	-I$(CURRENT_DIR) \
	-I$(BUILD_DIR) \
	-I$(MUTEK_SRC_DIR) \
	-include $(OBJ_DIR)/config.h

cflags:
	@echo $(CFLAGS) $(CPUCFLAGS) $(ARCHCFLAGS)

ifeq ($(TARGET_MK),)
TARGET_MK=flags.mk
endif

mkmf: $(TARGET_MK)

$(TARGET_MK): $(OBJ_DIR)/config.mk
	@> $@
	@echo 'CC=$(CC)' >> $@
	@echo 'LD=$(LD)' >> $@
	@echo 'AR=$(AR)' >> $@
	@echo 'AS=$(AS)' >> $@
	@echo 'OBJCOPY=$(OBJCOPY)' >> $@
	@echo 'OBJDUMP=$(OBJDUMP)' >> $@
	@echo 'CFLAGS=$(INCS) $(CFLAGS) $(CPUCFLAGS) $(ARCHCFLAGS)' >> $@
	@echo 'INCS=$(INCS)' >> $@
	@echo 'LDFLAGS=$(LINK_LDFLAGS) $(LDFLAGS) $(ARCHLDFLAGS) $(CPULDFLAGS)' >> $@

.SUFFIXES:
