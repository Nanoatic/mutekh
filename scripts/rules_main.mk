POST_TARGET=__foo.out

LDFLAGS=
TARGET_EXT ?= out

TARGET_SECTIONS=.boot .text .rodata .excep .cpudata .contextdata .data

LINKING=1
ifeq ($(TARGET_EXT),o)
LINKING=0
endif
export LINKING

OUT_NAME := $(shell cd $(MUTEK_SRC_DIR) ; \
		perl $(MUTEK_SRC_DIR)/scripts/config.pl	\
		--src-path=$(CONF_PATH):$(MUTEK_SRC_DIR):$(MUTEKH_CONFIG_PATH)  \
		--input=$(CONF)					 \
		--build-path=$(BUILD_DIR)/obj-   \
		--build=$(BUILD) --config $(CONFIG_FLAGS) )

ifeq ($(OUT_NAME),)
$(error Configure script failed)
endif

OBJ_DIR := $(BUILD_DIR)/obj-$(OUT_NAME)
target = $(subst /,-,$(OUT_NAME))
KERNEL_FILE=$(target).$(TARGET_EXT)
FINAL_KERNEL_FILE=$(target).$(TARGET_EXT)
LOG_FILE=$(OBJ_DIR)/build.log
#LOG_REDIR= 3>>$(LOG_FILE) 1>&3 2>&3

include $(OBJ_DIR)/config.mk

DEP_FILE_LIST:=

include $(MUTEK_SRC_DIR)/scripts/config.mk
include $(MUTEK_SRC_DIR)/scripts/discover.mk

ifneq ($(CLEANING),1)
define do_inc_dep

ifeq ($(wildcard $(1)),$(1))
include $(1)
# else
# $# $# (info $(1) not found)
endif

endef

$(eval \
$(foreach depfile,$(DEP_FILE_LIST),\
$(call do_inc_dep,$(depfile))))

$(eval $(call do_inc_dep,$(OBJ_DIR)/config.deps))

endif

TARGET_OBJECT_LIST:=$(filter %.o,$(TARGET_OBJECT_LIST))
META_OBJECT_LIST:=$(filter-out %ldscript,$(META_OBJECT_LIST))
COPY_OBJECT_LIST:=$(filter-out %ldscript,$(COPY_OBJECT_LIST))

all: kernel

$(OBJ_DIR)/.done_pre_header_list: $(PRE_HEADER_LIST)
	@touch $@

objs:
	echo "TARGET_OBJECT_LIST = $(TARGET_OBJECT_LIST)"
	echo "DEP_FILE_LIST = $(DEP_FILE_LIST)"
	echo "CLEAN_FILE_LIST = $(CLEAN_FILE_LIST)"
	echo "PRE_HEADER_LIST = $(PRE_HEADER_LIST)"

showpaths:
	@echo MUTEK_SRC_DIR $(MUTEK_SRC_DIR)
	@echo BUILD_DIR $(BUILD_DIR)
	@echo OBJ_DIR $(OBJ_DIR)
	@echo CONF $(CONF)
	@echo target $(target)
	@echo Modules: $(MODULES)
	@echo Module names: $(MODULE_NAMES)
	@echo Module src pahs:
	@$(foreach mn,$(MODULE_NAMES),echo " " $(mn): $($(mn)_SRC_DIR); )
	@echo Module build pahs:
	@$(foreach mn,$(MODULE_NAMES),echo " " $(mn): $($(mn)_OBJ_DIR); )

.PHONY: FORCE

FORCE:

kernel: $(OBJ_DIR)/$(KERNEL_FILE)
	cp $< $(BUILD_DIR)/$(FINAL_KERNEL_FILE)
ifeq ($(CONFIG_ARCH_EMU),defined)
	chmod +x $(BUILD_DIR)/$(FINAL_KERNEL_FILE)
endif
	@echo 'BUILD DIR   ' $(OBJ_DIR)
	@echo 'KERNEL      ' $(notdir $<)
	( echo ; echo Tools: ; echo ; ) >> $(LOG_FILE)
	$(CC) -v >> $(LOG_FILE) 2>&1 || true
	$(LD) -V >> $(LOG_FILE) 2>&1 || true
	echo >> $(LOG_FILE)
	( echo ; echo Repository: ; echo ; ) >> $(LOG_FILE)
	svn info $(MUTEK_SRC_DIR) >> $(LOG_FILE) || true
	echo >> $(LOG_FILE)
	touch $(OBJ_DIR)/build.env
	env | grep -v SESSION | grep -v PASS | sort > $(OBJ_DIR)/build.env_
	( echo ; echo Environment: ; echo ; ) >> $(LOG_FILE)
	diff $(OBJ_DIR)/build.env $(OBJ_DIR)/build.env_ >> $(LOG_FILE) || true
	mv $(OBJ_DIR)/build.env_ $(OBJ_DIR)/build.env
	( echo ; echo -n "================ finished on " ; date ; echo ) >> $(LOG_FILE)
	cp $(LOG_FILE) $(BUILD_DIR)/$(FINAL_KERNEL_FILE).log

clean:
	rm -f $(OBJ_DIR)/$(KERNEL_FILE) $(TARGET_OBJECT_LIST)
	rm -rf $(foreach mn,$(MODULE_NAMES),$($(mn)_OBJ_DIR))
	rm -f $(CONFIG_FILES)
	rm -f $(OBJ_DIR)/$(target).o
	rm -f $(OBJ_DIR)/$(target).out
	rm -f $(OBJ_DIR)/config.*
	rm -f $(OBJ_DIR)/.done_pre_header_list
	rm -f $(BUILD_DIR)/$(target).out

FINAL_LINK_TARGET?=$(OBJ_DIR)/$(target).out
FINAL_LINK_SOURCE?=$(OBJ_DIR)/$(target).o

ifeq ($(LD_NO_Q),1)

ifeq ($(CONFIG_ARCH_EMU_DARWIN),defined)
$(OBJ_DIR)/$(target).out: $(OBJ_DIR)/config.m4 \
		$(COPY_OBJECT_LIST) \
		$(META_OBJECT_LIST) \
		$(TARGET_OBJECT_LIST) \
	    FORCE
	echo '    LDL     $@' $(LOG_REDIR)
	$(LD) $(LINK_LDFLAGS) $(LDFLAGS) $(ARCHLDFLAGS) $(CPULDFLAGS) \
		$(filter %_before.o,$(filter %.o,$^)) \
		$(filter-out %_before.o %_after.o,$(filter %.o,$^)) \
		$(filter %.a,$^) \
		$(filter %_after.o,$(filter %.o,$^)) \
		-o $@ `$(CC) $(CFLAGS) $(CPUCFLAGS) -print-libgcc-file-name` \
	-flat_namespace \
	-e _arch_init \
	-undefined warning $(LOG_REDIR)

else
WL=-Wl,
$(OBJ_DIR)/$(target).out: $(OBJ_DIR)/config.m4 \
		$(COPY_OBJECT_LIST) \
		$(META_OBJECT_LIST) \
		$(TARGET_OBJECT_LIST) \
		$(arch_OBJ_DIR)/ldscript \
		$(cpu_OBJ_DIR)/ldscript \
	    FORCE
	@echo '    LDL     ' $(notdir $@) $(LOG_REDIR)
	$(CC) $(addprefix $(WL),$(LINK_LDFLAGS) $(LDFLAGS) $(ARCHLDFLAGS) $(CPULDFLAGS)) \
		$(CFLAGS) $(CPUCFLAGS) \
		$(filter %.o,$^) $(filter %.a,$^) \
		$(addprefix -T ,$(filter %ldscript,$^)) \
		-o $@ `$(CC) $(CFLAGS) $(CPUCFLAGS) -print-libgcc-file-name` $(LOG_REDIR)
endif
else
$(FINAL_LINK_TARGET): $(FINAL_LINK_SOURCE) FORCE \
		$(arch_OBJ_DIR)/ldscript \
		$(cpu_OBJ_DIR)/ldscript
	@echo '    LD out   ' $(notdir $@) $(LOG_REDIR)
	$(LD) $(LINK_LDFLAGS) $(LDFLAGS) $(ARCHLDFLAGS) $(CPULDFLAGS) \
		$< \
		-T $(arch_OBJ_DIR)/ldscript \
		-T $(cpu_OBJ_DIR)/ldscript \
		-o $@ $(LOG_REDIR)
endif

final_link: $(FINAL_LINK_TARGET)

$(OBJ_DIR)/$(target).o: $(OBJ_DIR)/config.m4 \
		$(COPY_OBJECT_LIST) \
		$(META_OBJECT_LIST) \
        $(TARGET_OBJECT_LIST) \
	    FORCE
	@echo '    LD o     ' $(notdir $@) $(LOG_REDIR)
	$(LD) -r \
		$(LDFLAGS) $(ARCHLDFLAGS) $(CPULDFLAGS) \
		-q $(filter %.o,$^) $(filter %.a,$^) \
		$(addprefix -T ,$(filter %ldscript,$^)) \
		-o $@ `$(CC) $(CFLAGS) $(CPUCFLAGS) -print-libgcc-file-name` $(LOG_REDIR)

$(OBJ_DIR)/$(target).pre.o: $(OBJ_DIR)/config.m4 $(TARGET_OBJECT_LIST) \
	    FORCE $(arch_SRC_DIR)/ldscript_obj
	@echo '    LD o     ' $(notdir $@) $(LOG_REDIR)
	$(LD) -r \
		$(LDFLAGS) $(ARCHLDFLAGS) $(CPULDFLAGS) \
		-q $(filter %.o,$^) $(filter %.a,$^) \
		-T $(arch_SRC_DIR)/ldscript_obj \
		-o $@ `$(CC) $(CFLAGS) $(CPUCFLAGS) -print-libgcc-file-name` $(LOG_REDIR)

kernel-postlink: $(POST_TARGET)

$(POST_TARGET): $(OBJ_DIR)/$(target).o $(POST_LDSCRIPT)
	@echo '    LD post ' $(notdir $@) $(LOG_REDIR)
	$(LD) -o $@ --gc-sections -T $(POST_LDSCRIPT) $< $(LOG_REDIR)

$(OBJ_DIR)/$(target).hex: $(OBJ_DIR)/$(target).out
	echo 'OBJCOPY HEX ' $(notdir $@) $(LOG_REDIR)
	$(OBJCOPY) $(addprefix -j ,$(TARGET_SECTIONS)) $(OBJCOPYFLAGS) -O ihex $< $@ $(LOG_REDIR)

$(OBJ_DIR)/$(target).bin: $(OBJ_DIR)/$(target).out
	echo 'OBJCOPY BIN ' $(notdir $@) $(LOG_REDIR)
	$(OBJCOPY) $(addprefix -j ,$(TARGET_SECTIONS)) $(OBJCOPYFLAGS) -O binary $< $@ $(LOG_REDIR)

