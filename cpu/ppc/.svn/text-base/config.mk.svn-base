CPUCFLAGS=-mstrict-align -fsigned-char -G0 -mcpu=405 -mno-dlmzb
CPUTOOLS=powerpc-unknown-elf-

CPUTOOLS_GCC_VERSION:=$(shell \
	(echo '((' ; $(CPUTOOLS)gcc -dumpversion | sed -e 's:\.)\?:)*100+:g' ) \
	)
CPUTOOLS_GCC_VERSION:=$(shell echo "$(CPUTOOLS_GCC_VERSION)" | bc -q)

# Dont forget the shell's 'true' is 0...
CPUTOOLS_GCC_4_4_PLUS:=$(shell test "$(CPUTOOLS_GCC_VERSION)" -lt "40400" ; echo "$$?" )

ifeq ($(CONFIG_COMPILE_DEBUG), defined)
ifeq ($(CPUTOOLS_GCC_4_4_PLUS), 1)
CPUCFLAGS += -fno-dwarf2-cfi-asm
endif
endif
