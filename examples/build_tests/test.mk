MUTEK_SRC_DIR?=$(abspath $(shell pwd)/../..)
CONFIGS = $(wildcard config_*)
KERNELS:=
BUILD_DIR?=$(shell pwd)
BUILD_DIRS:=

all: kernels

define declare_config

KERNELS+=build_$(1)/kernel
BUILD_DIRS+=$(BUILD_DIR)/build_$(1)

build_$(1):
	mkdir $$@

build_$(1)/kernel: build_$(1) $(1) FORCE
	$$(MAKE) -f $(MUTEK_SRC_DIR)/Makefile \
		MUTEK_SRC_DIR=$(MUTEK_SRC_DIR) \
		CONF=$$$${PWD}/$(1) \
		BUILD_DIR=$(BUILD_DIR)/build_$(1)

endef

$(eval $(foreach conf,$(filter-out $(DISABLED),$(CONFIGS)),$(call declare_config,$(conf))))

kernels: $(KERNELS)

clean:
	rm -rf $(BUILD_DIRS)

FORCE:

.PHONY: FORCE
