
objs = getline_lua.o mem.o

ifeq ($(CONFIG_VFS),defined)
objs += vfs_functions.o
endif

ifeq ($(CONFIG_LIBELF_RTLD),defined)
objs += rtld_functions.o
endif

ifeq ($(CONFIG_LIBDSRL),defined)
objs += dsrl_functions.o
endif

ifeq ($(CONFIG_LIBCRYPTO_MD5),defined)
objs += crypto_functions.o
endif

ifeq ($(CONFIG_ARCH_SIMPLE_OLIMEX_SAM7_EX256),defined)
ifeq ($(CONFIG_DRIVER_LCD),defined)
objs += lcd.o
endif
endif

ifeq ($(CONFIG_MUTEK_TIMERMS),defined)
objs += timer.o
endif

#DIR_CFLAGS=-O0 -ggdb
