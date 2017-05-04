
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/types.h>
#include "elf.h"

#ifdef __linux__

#  include <endian.h>

#elif defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)

#  include <machine/endian.h>
#  define __BYTE_ORDER BYTE_ORDER
#  define __LITTLE_ENDIAN LITTLE_ENDIAN
#  define __BIG_ENDIAN BIG_ENDIAN

#else

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321

#  ifdef __LITTLE_ENDIAN__
#    define __BYTE_ORDER __LITTLE_ENDIAN
#  endif

#  if defined(i386) || defined(__i386__)
#    define __BYTE_ORDER __LITTLE_ENDIAN
#  endif

#  if defined(sun) && defined(unix) && defined(sparc)
#    define __BYTE_ORDER __BIG_ENDIAN
#  endif

#endif /* os switches */

#ifndef __BYTE_ORDER
# error Need to know endianess
#endif


#if __BYTE_ORDER == __LITTLE_ENDIAN
# define uint32_le_to_machine(x) (x)
# define uint32_machine_to_le(x) (x)
# define uint32_be_to_machine(x) uint32_swap(x)
# define uint32_machine_to_be(x) uint32_swap(x)

# define uint16_le_to_machine(x) (x)
# define uint16_machine_to_le(x) (x)
# define uint16_be_to_machine(x) uint16_swap(x)
# define uint16_machine_to_be(x) uint16_swap(x)
#else
# define uint32_le_to_machine(x) uint32_swap(x)
# define uint32_machine_to_le(x) uint32_swap(x)
# define uint32_be_to_machine(x) (x)
# define uint32_machine_to_be(x) (x)

# define uint16_le_to_machine(x) uint16_swap(x)
# define uint16_machine_to_le(x) uint16_swap(x)
# define uint16_be_to_machine(x) (x)
# define uint16_machine_to_be(x) (x)
#endif

static inline uint32_t uint32_swap(uint32_t x)
{
    return (
        ( (x & 0xff)   << 24 ) |
        ( (x & 0xff00) <<  8 ) |
        ( (x >>  8) & 0xff00 ) |
        ( (x >> 24) &   0xff )
        );
}

static inline uint16_t uint16_swap(uint16_t x)
{
    return ((x << 8) | (x >> 8));
}


#define swap(size, endian, var, member)								   \
	var->member = uint##size##_##endian##_to_machine(var->member)

int elf_check_header(const Elf32_Hdr *header, uint8_t class)
{
	return (
		(header->e_ident[EI_MAG0] == ELFMAG0) &&
		(header->e_ident[EI_MAG1] == ELFMAG1) &&
		(header->e_ident[EI_MAG2] == ELFMAG2) &&
		(header->e_ident[EI_MAG3] == ELFMAG3) &&
		(header->e_ident[EI_CLASS] == class)
		);
}

void elf_swap_header(Elf32_Hdr *header)
{
	switch ( header->e_ident[EI_DATA] ) {
	case ELFDATA2LSB:
		swap(16, le, header, e_type);
		swap(16, le, header, e_machine);
		swap(32, le, header, e_version);
		swap(32, le, header, e_entry);
		swap(32, le, header, e_phoff);
		swap(32, le, header, e_shoff);
		swap(32, le, header, e_flags);
		swap(16, le, header, e_ehsize);
		swap(16, le, header, e_phentsize);
		swap(16, le, header, e_phnum);
		swap(16, le, header, e_shentsize);
		swap(16, le, header, e_shoff);
		swap(16, le, header, e_shstrndx);
		break;
	case ELFDATA2MSB:
		swap(16, be, header, e_type);
		swap(16, be, header, e_machine);
		swap(32, be, header, e_version);
		swap(32, be, header, e_entry);
		swap(32, be, header, e_phoff);
		swap(32, be, header, e_shoff);
		swap(32, be, header, e_flags);
		swap(16, be, header, e_ehsize);
		swap(16, be, header, e_phentsize);
		swap(16, be, header, e_phnum);
		swap(16, be, header, e_shentsize);
		swap(16, be, header, e_shoff);
		swap(16, be, header, e_shstrndx);
		break;
	case ELFDATANONE:
		break;
	default:
		assert(0);
	}
}

void elf_swap_pheader(const Elf32_Hdr *header, Elf32_Phdr *pheader)
{
	switch ( header->e_ident[EI_DATA] ) {
	case ELFDATA2LSB:
		swap(32, le, pheader, p_type);
		swap(32, le, pheader, p_offset);
		swap(32, le, pheader, p_vaddr);
		swap(32, le, pheader, p_paddr);
		swap(32, le, pheader, p_filesz);
		swap(32, le, pheader, p_memsz);
		swap(32, le, pheader, p_flags);
		swap(32, le, pheader, p_align);
		break;
	case ELFDATA2MSB:
		swap(32, be, pheader, p_type);
		swap(32, be, pheader, p_offset);
		swap(32, be, pheader, p_vaddr);
		swap(32, be, pheader, p_paddr);
		swap(32, be, pheader, p_filesz);
		swap(32, be, pheader, p_memsz);
		swap(32, be, pheader, p_flags);
		swap(32, be, pheader, p_align);
		break;
	case ELFDATANONE:
		break;
	default:
		assert(0);
	}
}

