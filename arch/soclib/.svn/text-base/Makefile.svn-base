
objs = 											\
arch_init.o										\
mem_init.o

ifneq ($(CONFIG_SOCLIB_EARLY_CONSOLE),undefined)
objs += early_console.o
endif

ifeq ($(CONFIG_HEXO_MMU), defined)
objs += mmu.o
endif

ifeq ($(CONFIG_SOCLIB_MEMCHECK), defined)
 objs += mem_checker.o
endif

ifeq ($(LINKING),1)

meta = ldscript

endif
