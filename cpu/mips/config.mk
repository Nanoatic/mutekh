ifeq ($(CONFIG_CPU_MIPS_VERSION), 32)
CPUCFLAGS=-mips32
endif

ifeq ($(CONFIG_CPU_MIPS_VERSION), 322)
CPUCFLAGS=-mips32r2
endif

ifeq ($(CONFIG_CPU_MIPS_VERSION), 4)
CPUCFLAGS=-mips4 -mno-branch-likely
endif

ifeq ($(CONFIG_CPU_MIPS_VERSION), 3)
CPUCFLAGS=-mips3 -mno-branch-likely
endif

ifeq ($(CONFIG_CPU_MIPS_VERSION), 2)
CPUCFLAGS=-mips2 -mno-branch-likely
endif

ifeq ($(CONFIG_CPU_MIPS_VERSION), 1)
CPUCFLAGS=-mips1 -mno-branch-likely
endif

CPUTOOLS=mipsel-unknown-elf-

ifeq ($(CONFIG_CPU_ENDIAN_LITTLE), defined)
CPUCFLAGS+= -EL
CPULDFLAGS+= -EL
endif

ifeq ($(CONFIG_CPU_ENDIAN_BIG), defined)
CPUCFLAGS+= -EB
CPULDFLAGS+= -EB
endif

CPUCFLAGS+= -G0

