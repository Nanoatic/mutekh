
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "elf.h"

int main(int argc, char **argv)
{
	if ( argc < 2 )
		return 1;

	int fd = open(argv[1], O_RDONLY);
	int i;

	Elf32_Hdr header;
	if ( read(fd, &header, sizeof(header)) < 0 ) {
		perror(argv[1]);
		return 1;
	}

	if ( !elf_check_header(&header, ELFCLASS32) ) {
		printf("Wrong file type\n");
		return 2;
	}

	printf("Opened \"%s\" elf32 file\n", argv[1]);

	elf_swap_header(&header);

	for ( i=0; i<header.e_phnum; ++i ) {
		Elf32_Phdr ph;
		int flags = 0;

		lseek(fd, header.e_phoff+sizeof(ph)*i, SEEK_SET);

		if ( read(fd, &ph, sizeof(ph)) < 0 ) {
			perror(argv[1]);
			return 1;
		}
		elf_swap_pheader(&header, &ph);

		if ( ph.p_type != PT_LOAD )
			continue;

		char flagstr[4] = "   ";

		if ( ph.p_flags & PF_X ) {
			flags |= PROT_EXEC;
			flagstr[2] = 'x';
		}
		if ( ph.p_flags & PF_R ) {
			flags |= PROT_READ;
			flagstr[0] = 'r';
		}
		if ( ph.p_flags & PF_W ) {
			flags |= PROT_WRITE;
			flagstr[1] = 'w';
		}

		printf("Segment @%p from file offset %p, fsize=%p, msize=%p [%s]\n",
			   (void*)ph.p_paddr, (void*)ph.p_offset,
			   (void*)ph.p_filesz, (void*)ph.p_memsz,
			   flagstr);


		void* ret = mmap((void*)ph.p_paddr, ph.p_memsz, 
						 PROT_WRITE,
						 MAP_FIXED|MAP_PRIVATE|MAP_ANON,
						 0, 0);
		if ( ret == MAP_FAILED ) {
			perror("mmap");
			return 1;
		}
		assert( ret == (void*)ph.p_paddr );
		memset(ret, 0, ph.p_memsz);
		lseek(fd, ph.p_offset, SEEK_SET);
		read(fd, ret, ph.p_filesz);
		mprotect(ret, ph.p_memsz, flags);
	}

	typedef int (entry_t)();
	entry_t *func = (entry_t*)header.e_entry;
	return func();
}
