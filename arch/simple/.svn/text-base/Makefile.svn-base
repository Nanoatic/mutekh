
objs =											\
arch_init.o										\
mem_init.o \
hw_init.o

ifeq ($(CONFIG_ARCH_SIMPLE_SAM7),defined)
subdirs+=sam7
endif

ifeq ($(CONFIG_ARCH_SIMPLE_ATMEGA),defined)
subdirs+=atmega
endif

meta = ldscript

