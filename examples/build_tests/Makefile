
objs = hello.o

ifneq ($(CONFIG_MUTEK_MAIN),defined)
objs += start.o
endif

ifeq ($(CONFIG_ARCH_HW_INIT_USER),defined)
objs += hw_init.o
endif

ifeq ($(CONFIG_ARCH_SOCLIB),defined)
ifeq ($(CONFIG_ARCH_DEVICE_TREE),defined)

objs += platform-$(CONFIG_CPU_NAME).o

endif
endif
