
objs =											\
boot.o											\
cpu_init.o										\
cpu_context.o                                                                           \
interrupts.o

ifeq ($(CONFIG_HEXO_MMU), defined)
objs += mmu.o
endif

subdirs = libc

ifeq ($(CONFIG_LIBELF), defined)
subdirs += libelf
endif

ifeq ($(CONFIG_NETWORK), defined)
subdirs += libnetwork
endif

ifeq ($(CONFIG_LIBUNIX), defined)
subdirs += libunix
endif

copy = ldscript

