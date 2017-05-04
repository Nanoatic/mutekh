
objs =											\
arch_init.o										\
mem_init.o \
hw_init.o

ifeq ($(CONFIG_ARCH_IBMPC_DMA), defined)
objs += dma-8237.o
endif

ifeq ($(CONFIG_IBMPC_EARLY_CONSOLE_VGA), defined)
objs += early_console.o
endif

meta = ldscript
