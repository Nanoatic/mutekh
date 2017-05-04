
objs =											\
arch_init.o										\
mem_init.o \
hw_init.o

ifeq ($(CONFIG_ARCH_EMU_DARWIN),defined)
objs += darwin_before.o darwin_after.o
endif

ifeq ($(CONFIG_HEXO_IRQ),defined)
objs += interrupts.o
endif

subdirs = include/arch/hexo

ifeq ($(LINKING),1)
meta = ldscript
endif
