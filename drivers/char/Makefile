
ifeq ($(CONFIG_DRIVER_CHAR), defined)
 objs += device_char.o
endif

ifeq ($(CONFIG_DRIVER_CHAR_SOCLIBTTY), defined)
 subdirs += tty-soclib
endif

ifeq ($(CONFIG_DRIVER_CHAR_VGATTY), defined)
 subdirs += tty-vga
endif

ifeq ($(CONFIG_DRIVER_CHAR_EMUTTY), defined)
 subdirs += tty-emu
endif

ifeq ($(CONFIG_DRIVER_CHAR_US6089C_UART), defined)
 subdirs += uart-us6089c
endif

ifeq ($(CONFIG_DRIVER_CHAR_SAM7DBGU), defined)
 subdirs += dbgu-sam7
endif

ifeq ($(CONFIG_DRIVER_CHAR_UART8250), defined)
 subdirs += uart-8250
endif

ifeq ($(CONFIG_DRIVER_CHAR_RANDOM), defined)
 subdirs += random
endif

ifeq ($(CONFIG_DRIVER_CHAR_ZERO), defined)
 subdirs += zero
endif

ifeq ($(CONFIG_DRIVER_CHAR_NULL), defined)
 subdirs += null
endif

