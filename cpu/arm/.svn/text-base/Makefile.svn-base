
objs = 											\
cpu_init.o										\
cpu_context.o

ifeq ($(CONFIG_CPU_RESET_HANDLER),defined)
objs += boot.o exception.o
endif

copy = ldscript

subdirs = libc

ifeq ($(CONFIG_CPU_ARM_THUMB), defined)
objs += thumb_helpers.o
exception.o_CFLAGS = -mno-thumb
cpu_init.o_CFLAGS = -mno-thumb
boot_m.o_CFLAGS = -mno-thumb
boot.o_CFLAGS = -mno-thumb
thumb_helpers.o_CFLAGS = -mno-thumb
endif
