
objs = 											\
exception.o										\
cpu_init.o										\
cpu_context.o

ifeq ($(CONFIG_CPU_RESET_HANDLER),defined)
objs += boot.o
endif

copy = ldscript

subdirs = libc
