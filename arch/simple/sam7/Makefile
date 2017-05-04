
objs=sam7.o

ifeq ($(CONFIG_ARCH_SIMPLE_OLIMEX_SAM7_EX256),defined)
objs += sam7_ex256_hw_init.o
endif

ifeq ($(CONFIG_CPU_ARM_THUMB), defined)
sam7.o_CFLAGS = -mno-thumb
endif
