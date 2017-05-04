
ifeq ($(shell uname -s), Darwin)
CPUTOOLS=i686-unknown-elf-
DEPCC=gcc
else
CPUTOOLS=
endif

CPUCFLAGS=-mno-tls-direct-seg-refs -m32 -malign-double
CPULDFLAGS= -melf_i386

