
MODULES = $(BUILD_MODULES) $(foreach mod,$(BASE_MODULES),$(mod):$(MUTEK_SRC_DIR)/$(mod))

# filter module names
MODULE_NAMES := $(foreach modwd,$(MODULES),$(shell echo $(modwd) | cut -d: -f1))

export MODULE_NAMES

# for all modules looking like module_name:module_src_dir, export
# module_name_SRC_DIR := module_src_dir
# module_name_OBJ_DIR := BUILD_DIR/module_src_dir/obj-arch-cpu

define declare_module_dir

MKDOC_ARGS += -I $(2)/include
$(1)_SRC_DIR:=$(2)
$(1)_OBJ_DIR:=$(OBJ_DIR)/$(1)

endef

$(eval \
$(foreach modwd,$(MODULES),\
$(call declare_module_dir,$(shell echo $(modwd) | cut -d: -f1),$(shell echo $(modwd) | cut -d: -f2))))


# cflags need to parse cpu/xxx/config.mk and arch/xxx/config.mk, but
# they can't know until now (when cpu_SRC_DIR and arch_SRC_DIR are
# defined).

include $(MUTEK_SRC_DIR)/scripts/cflags.mk




META_OBJECT_LIST:=
COPY_OBJECT_LIST:=
TARGET_OBJECT_LIST:=
PRE_HEADER_LIST:=
CLEAN_FILE_LIST:=
GLOBAL_DOC_HEADERS:=

include $(MUTEK_SRC_DIR)/scripts/local.mk

$(eval \
$(foreach modwd,$(MODULE_NAMES),\
$(call scan_local_makefile,$($(modwd)_SRC_DIR),$($(modwd)_OBJ_DIR))))
