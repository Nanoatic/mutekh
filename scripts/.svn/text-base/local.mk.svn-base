
ifdef HETLINK
define do_hetlink_mangling
	md5=$(shell md5sum $< | cut -c 1-8) ; \
		rm -f $@.static ; \
		$(CPUTOOLS)nm $@ | grep ' t ' | cut -c 12- | sort -u | while read i ; do echo "$${i} _$${md5}_$${i}" >> $@.static ; done
	if test -e $@.static ; then \
		echo '             renaming static symbols' $(LOG_REDIR) ; \
		$(CPUTOOLS)objcopy --redefine-syms=$@.static $@ $(LOG_REDIR) ; \
	fi
endef
endif

# prepare_command msg file
# creates the output directory parent of file
define prepare_command
	@echo '$(1)	$$(notdir $(value 2))' $$(LOG_REDIR)
	(test -d $$(dir $(value 2)) || mkdir -p $$(dir $(value 2))) $$(LOG_REDIR)
endef

# run_command dest_file cmd
# runs a command in destination directory, logging the output
define run_command
	( cd $$(dir $(value 1)) ; \
	    $(value 2) \
	) $(LOG_REDIR)
endef

# compute_depfile_c depfile target input [flags]
# runs gcc -M to compute dependancy makefile
define compute_depfile_c
	( cd $$(dir $(value 1)) ; \
		$(DEPCC) \
			$$(CFLAGS) $$(DEPINC) $(value 4) \
			-M -MT $(value 2) -MF $(value 1) $(value 3) \
	) $(LOG_REDIR)
endef

# compile compiler target input [flags]
# runs compiler to build object
define compile
	( cd $$(dir $(value 2)) ; \
		$(value 1) -c  \
			$$(CFLAGS) $$(CPUCFLAGS) $$(ARCHCFLAGS) $$(INCS) \
			$(value 4) $(value 3) -o $(value 2) \
	) $(LOG_REDIR)
endef

# blob2c c_file blob symbol_name
# runs blob2c.py to build c file
define blob2c
	( cd $$(dir $(value 1)) ; \
		python $(MUTEK_SRC_DIR)/scripts/blob2c.py \
	    -a 4 -o $(value 1) -S -n $(value 3) $(value 2) \
	) $(LOG_REDIR)
endef

## declare_copy: file_name, src_dir, obj_dir

define declare_copy

#$( # info  ======== declare_copy, $(1), $(2), $(3))

$(3)/$(1): $(2)/$(1)
	$(call prepare_command,COPY,$$@)
	$(call run_command,$$@,cp $$< $$@)

endef



## declare_obj: file_name, src_dir, obj_dir

define declare_obj

DEP_FILE_LIST+=$(3)/$(1:.o=.deps)

#$( # info  ======== declare_obj, $(1), $(2), $(3))

ifeq ($(wildcard $(2)/$(1:.o=.S.m4)),$(2)/$(1:.o=.S.m4))

#$$( # info  ======== declare_obj, $(1), $(2), $(3), found to be M4+ASM file)

DEP_FILE_LIST+=$(3)/$(1:.o=.m4.deps)

$(3)/$(1): $(2)/$(1:.o=.S.m4) $(MUTEK_SRC_DIR)/scripts/global.m4 $(OBJ_DIR)/config.m4 $(MUTEK_SRC_DIR)/scripts/compute_m4_deps.py
	$(call prepare_command,M4+AS,$$@)
	cat $(MUTEK_SRC_DIR)/scripts/global.m4 $(OBJ_DIR)/config.m4 \
		$$< | m4 -s $$(filter -I%,$$(INCS)) -P | \
		python $(MUTEK_SRC_DIR)/scripts/compute_m4_deps.py \
		$$@ $$(filter -I%,$$(INCS)) > $$(@:.o=.m4.deps)
	cat $(MUTEK_SRC_DIR)/scripts/global.m4 $(OBJ_DIR)/config.m4 \
		$$< | m4 $$(filter -I%,$$(INCS)) -P > $$(@:.o=.S)
	$(call compute_depfile_c,$$(@:.o=.deps),$(3)/$(1),$$(@:.o=.S))
	$(call run_command,$$@,$(CPP) $$(INCS) $$(@:.o=.S) | $(AS) $$(CPUASFLAGS) -o $$@)

else ifeq ($(wildcard $(2)/$(1:.o=.S)),$(2)/$(1:.o=.S))

#$$( # info  ======== declare_obj, $(1), $(2), $(3), found to be ASM file)

$(3)/$(1): $(2)/$(1:.o=.S) $(OBJ_DIR)/.done_pre_header_list $(OBJ_DIR)/config.h
	$(call prepare_command,AS,$$@)
	$(call compute_depfile_c,$$(@:.o=.deps),$(3)/$(1),$$<)
	$(call run_command,$$@,$(CC) $$(INCS) -c -x assembler-with-cpp $$< $$(CPUCFLAGS) -o $$@)

else ifeq ($(wildcard $(2)/$(1:.o=.dts)),$(2)/$(1:.o=.dts))

#$$( # info  ======== declare_obj, $(1), $(2), $(3), found to be a device-tree file)

$(3)/$(1): $(2)/$(1:.o=.dts) $(OBJ_DIR)/config.h
	$(call prepare_command,DTC,$$@)
	$(call run_command,$$@,$(DTC) -O dtb -o $(3)/$(1:.o=.blob) $$<)
	$(call blob2c,$$(@:.o=.c),$$(@:.o=.blob),dt_blob_start)
	$(call compile,$(CC),$$@,$$(@:.o=.c))

else ifeq ($(wildcard $(2)/$(1:.o=.dict)),$(2)/$(1:.o=.dict))

#$$( # info  ======== declare_obj, $(1), $(2), $(3), found to be a forth dictionary)

$(3)/$(1): $(2)/$(1:.o=.dict)
	$(call prepare_command,DICT,$$@)
	$(call blob2c,$$(@:.o=.c),$$<,forth_dictionary)
	$(call compile,$(CC),$$@,$$(@:.o=.c))

else ifeq ($(wildcard $(2)/$(1:.o=.cc)),$(2)/$(1:.o=.cc))

#$$( # info  ======== declare_obj, $(1), $(2), $(3), found to be C++ file)

$(3)/$(1): $(2)/$(1:.o=.cc) $(OBJ_DIR)/config.h $(OBJ_DIR)/.done_pre_header_list
	$(call prepare_command,C++,$$@)
	$(call compute_depfile_c,$$(@:.o=.deps),$(3)/$(1),$$<,$(CPUCFLAGS) $(ARCHCFLAGS) $(INCS) \
		$($(1)_CXXFLAGS) $(DIR_CXXFLAGS))
	$(call compile,$(CXX),$$@,$$<,$($(1)_CXXFLAGS) $(DIR_CXXFLAGS))
	$(value do_hetlink_mangling)

else ifeq ($(wildcard $(2)/$(1:.o=.cpp)),$(2)/$(1:.o=.cpp))

#$$( # info  ======== declare_obj, $(1), $(2), $(3), found to be C++ file)

$(3)/$(1): $(2)/$(1:.o=.cpp) $(OBJ_DIR)/config.h $(OBJ_DIR)/.done_pre_header_list
	$(call prepare_command,C++,$$@)
	$(call compute_depfile_c,$$(@:.o=.deps),$(3)/$(1),$$<,$(CPUCFLAGS) $(ARCHCFLAGS) $(INCS) \
		$($(1)_CXXFLAGS) $(DIR_CXXFLAGS))
	$(call compile,$(CXX),$$@,$$<,$($(1)_CXXFLAGS) $(DIR_CXXFLAGS))
	$(value do_hetlink_mangling)

else

#$$( # info  ======== declare_obj, $(1), $(2), $(3), found to be C file)

$(3)/$(1): $(2)/$(1:.o=.c) $(OBJ_DIR)/config.h $(OBJ_DIR)/.done_pre_header_list
	$(call prepare_command,CC,$$@)
	$(call compute_depfile_c,$$(@:.o=.deps),$(3)/$(1),$$<,$(CPUCFLAGS) $(ARCHCFLAGS) $(INCS) \
		$($(1)_CFLAGS) $(DIR_CFLAGS))
	$(call compile,$(CC),$$@,$$<,$($(1)_CFLAGS) $(DIR_CFLAGS))
	$(value do_hetlink_mangling)

endif

endef

define declare_gpct_header

$(3)/$(1): $(2)/$(1:.h=.t)
	$(call prepare_command,\\,$$@)
	cp $$< $$@ $(LOG_REDIR)
	perl $(MUTEK_SRC_DIR)/gpct/gpct/build/backslash.pl < $$< > $$@ 2>> $(LOG_FILE)
#	sed -e 's!^warning:\([0-9]*\):!$$<:\1:warning:!g' < $(LOG_FILE) 1>&2

endef

## declare_meta_h: file_name, src_dir, obj_dir

define declare_meta_h

#$( # info  ======== declare_meta_h, $(1), $(2), $(3))

# Extract HOST defined macros and inject values in a new header file.
# This is used by emultaion platform to get correct syscall numbers and args
$(3)/$(1): $(2)/$(1:.h=.def) $(OBJ_DIR)/config.h
	$(call prepare_command,H_CPP,$$@)
	cat $(OBJ_DIR)/config.h $(2)/$(1:.h=.def) | \
		$(HOSTCC) $$(CFLAGS) $$(CPUCFLAGS) $$(ARCHCFLAGS) -E - | grep '#define' > $(3)/$(1)

endef

## declare_meta_cpp: file_name, src_dir, obj_dir

define declare_meta_cpp

#$( # info  ======== declare_meta_cpp, $(1), $(2), $(3))

DEP_FILE_LIST+=$(3)/$(1).deps

ifeq ($(wildcard $(2)/$(1).cpp),$(2)/$(1).cpp)

# cpp preprocessed files
$(3)/$(1): $(2)/$(1).cpp $(OBJ_DIR)/config.h
	$(call prepare_command,CPP,$$@)
	$(DEPCC) -E -M -MF $$@.deps -MT $$@ $$(INCS) -P -x c $$<
	$(CC) -E $$(INCS) -P -x c - < $$< > $$@

else

# m4 preprocessed files
$(3)/$(1): $(2)/$(1).m4 $(OBJ_DIR)/config.m4 $(MUTEK_SRC_DIR)/scripts/global.m4 $(MUTEK_SRC_DIR)/scripts/compute_m4_deps.py
	$(call prepare_command,M4,$$@)
	cat $(MUTEK_SRC_DIR)/scripts/global.m4 $(OBJ_DIR)/config.m4 \
		$$< | m4 -s $$(filter -I%,$$(INCS)) -P | \
		python $(MUTEK_SRC_DIR)/scripts/compute_m4_deps.py \
		$$@ $$(filter -I%,$$(INCS)) > $$@.deps
	cat $(MUTEK_SRC_DIR)/scripts/global.m4 $(OBJ_DIR)/config.m4 \
		$$< | m4 $$(filter -I%,$$(INCS)) -P > $$@

endif

endef

define declare_doc_header

MKDOC_ARGS += $(1)

endef

define declare_doc_files

MKDOC_ARGS += $(2)/$(1)

endef

## scan_local_makefile: src_dir, obj_dir

define scan_local_makefile

LOCAL_SRC_DIR:=$(1)
srcdir:=$(1)
LOCAL_OBJ_DIR:=$(2)
LOCAL_PPRINT_DIR:=$$(subst $(PWD)/,,$$(LOCAL_OBJ_DIR))

# $$( # info  ======== \
# 	scan_local_makefile \
# 	"LOCAL_SRC_DIR=$$(LOCAL_SRC_DIR)" \
# 	"LOCAL_OBJ_DIR=$$(LOCAL_OBJ_DIR)" \
# 	"LOCAL_PPRINT_DIR=$$(LOCAL_PPRINT_DIR)")

objs:=
meta:=
copy:=
subdirs:=
pre_headers:=
doc_headers:=
doc_files:=

include $$(LOCAL_SRC_DIR)/Makefile

#$$( # info  OBJS=$$(objs))

TARGET_OBJECT_LIST+=$$(addprefix $$(LOCAL_OBJ_DIR)/,$$(objs))
COPY_OBJECT_LIST+=$$(addprefix $$(LOCAL_OBJ_DIR)/,$$(copy))
META_OBJECT_LIST+=$$(addprefix $$(LOCAL_OBJ_DIR)/,$$(meta))
PRE_HEADER_LIST+=$$(addprefix $$(LOCAL_OBJ_DIR)/,$$(pre_headers))
CLEAN_FILE_LIST+=$$(addprefix $$(LOCAL_OBJ_DIR)/,$$(objs) $$(copy) $$(meta))

PRE_HEADER_LIST+=$$(filter %.h,$$(COPY_OBJECT_LIST))

$$(LOCAL_OBJ_DIR):
	mkdir -p $$@

$$(eval $$(foreach obj,$$(objs),$$(call declare_obj,$$(obj),$$(LOCAL_SRC_DIR),$$(LOCAL_OBJ_DIR))))

$$(eval $$(foreach tocopy,$$(copy),$$(call declare_copy,$$(tocopy),$$(LOCAL_SRC_DIR),$$(LOCAL_OBJ_DIR))))

$$(eval $$(foreach tometa,$$(filter %.h,$$(meta)),$$(call declare_meta_h,$$(tometa),$$(LOCAL_SRC_DIR),$$(LOCAL_OBJ_DIR))))

$$(eval $$(foreach tometa,$$(filter-out %.h,$$(meta)),$$(call declare_meta_cpp,$$(tometa),$$(LOCAL_SRC_DIR),$$(LOCAL_OBJ_DIR))))

$$(eval $$(foreach ph,$$(pre_headers),$$(call declare_gpct_header,$$(ph),$$(LOCAL_SRC_DIR),$$(LOCAL_OBJ_DIR))))

$$(eval \
$$(foreach h,$$(doc_headers),\
$$(call declare_doc_header,$$(h))))

$$(eval \
$$(foreach f,$$(doc_files),\
$$(call declare_doc_files,$$(f),$$(LOCAL_SRC_DIR))))

# Beware this must be left last in calls

$$(eval \
$$(foreach m,$$(subdirs),\
$$(call scan_local_makefile,$$(LOCAL_SRC_DIR)/$$(m),$$(LOCAL_OBJ_DIR)/$$(m))))

endef

