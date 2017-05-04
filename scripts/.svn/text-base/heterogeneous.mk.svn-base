
HETLINK=$(MUTEK_SRC_DIR)/tools/hlink/hetlink
BUILDS:=$(subst :, ,$(EACH))

CLEAN_TARGETS=$(foreach build,$(BUILDS),$(BUILD_DIR)/het-kernel-$(build)-clean)
PRE_OBJS=$(foreach build,$(BUILDS),$(BUILD_DIR)/het-kernel-$(build).pre.o)
HET_OBJS=$(foreach build,$(BUILDS),$(BUILD_DIR)/het-kernel-$(build).pre.o.het.o)
HET_KERNELS=$(foreach build,$(BUILDS),$(BUILD_DIR)/het-kernel-$(build).het.out)

export HETLINK

kernel-het: kernel

kernel: $(HET_KERNELS)
#	echo "BUILDS: $(BUILDS)"
	echo "HET_KERNELS: "$(notdir $(HET_KERNELS))
#	echo "PRE_OBJS: $(PRE_OBJS)"
#	echo "HET_OBJS: $(HET_OBJS)"

$(BUILD_DIR)/het-kernel-%.pre.o: FORCE
	@echo "PRE $@"
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/rules_main.mk \
		 MAKEFLAGS=$(MAKEFLAGS) \
	     CONF=$(CONF) \
	     BUILD=$(BUILD):$* \
		 BUILD_DIR=$(BUILD_DIR) TARGET_EXT=pre.o \
		 target=het-kernel-$* \
		 kernel

$(BUILD_DIR)/het-kernel-%-clean: FORCE
	@echo "CLEAN $@"
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/rules_main.mk \
		 MAKEFLAGS=$(MAKEFLAGS) \
	     CONF=$(CONF) \
	     BUILD=$(BUILD):$* \
		 BUILD_DIR=$(BUILD_DIR) TARGET_EXT=pre.o \
		 target=het-kernel-$* \
		 clean

# We have to go through an unique target or the hetlink will be done
# twice...

$(HET_OBJS): __do_hetlink FORCE

.NOPARALLEL: __do_hetlink

__do_hetlink : $(PRE_OBJS) $(HETLINK_CONF) FORCE
	echo '    HETLINK ' $(notdir $@)
	$(HETLINK) -v 4 -c $(MUTEK_SRC_DIR)/scripts/hetlink.conf $(PRE_OBJS)

$(BUILD_DIR)/het-kernel-%.het.out : $(BUILD_DIR)/het-kernel-%.pre.o.het.o FORCE
	$(MAKE) -f $(MUTEK_SRC_DIR)/scripts/rules_main.mk \
		 MAKEFLAGS=$(MAKEFLAGS) \
		 CONF=$(CONF) CONFIG_FLAGS=--quiet \
		 BUILD_DIR=$(BUILD_DIR) \
		 BUILD=$(BUILD):$* \
		 FINAL_LINK_TARGET=$@ \
		 FINAL_LINK_SOURCE=$< \
		 final_link



clean: $(CLEAN_TARGETS)
	rm -f $(PRE_OBJS) $(HET_OBJS) $(HET_KERNELS)

.PHONY: FORCE

FORCE:

