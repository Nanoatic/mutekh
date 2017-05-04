
objs = elf.o 

ifeq ($(CONFIG_LIBELF_RTLD), defined)
  objs += rtld.o user.o
endif

ifeq ($(CONFIG_LIBELF_RTLD_TLS), defined)
  objs += tls.o
endif

ifeq ($(CONFIG_LIBELF_RTLD_KERNEL), defined)
  objs += kernel.o
endif

doc_headers = libelf/elf.h \
			  libelf/rtld.h
