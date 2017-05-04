
ARCHCFLAGS= -static
ARCHLDFLAGS= -static

ifeq ($(CONFIG_ARCH_EMU_DARWIN), defined)
HOSTCPPFLAGS= -I/System/Library/Frameworks/Kernel.framework/Headers
LD_NO_Q = 1
ARCHCFLAGS+=-fno-pic
endif


