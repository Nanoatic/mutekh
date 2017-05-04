/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright (c) UPMC, Lip6, STMicroelectronics
        Joel Porquet <joel.porquet@lip6.fr>, 2009
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <mutek/mem_alloc.h>
#include <hexo/endian.h>

#include <libelf/elf.h>

#define ELF_HDR_SIZE 128 /* based on linux, the header should be less than 128bytes */

/* Load ELF header, perform test and load Program Header
 *
 * @param file File descriptor on the ELF file
 * @param elfobj Elf object
 * @return error_t Error code if any
 */
static error_t _elf_load_headers(FILE *file, struct obj_elf_s *elfobj)
{
    elf_ehdr_t *ehdr;
    uint8_t buf[ELF_HDR_SIZE];

    elf_phdr_t *p;

    _libelf_debug(DEBUG, "_elf_load_headers\n");

    /* Get the elf header */
    if (fread(buf, sizeof(*buf), ELF_HDR_SIZE, file) != ELF_HDR_SIZE) 
    {
        _libelf_debug(NONE, "\tError while reading elf file\n");
        goto err;
    }

    ehdr = (elf_ehdr_t*)buf;

    /* Make sure the file is valid */
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0
            || ehdr->e_ident[EI_MAG1] != ELFMAG1
            || ehdr->e_ident[EI_MAG2] != ELFMAG2
            || ehdr->e_ident[EI_MAG3] != ELFMAG3) 
    {
        _libelf_debug(NONE, "\tinvalid file format\n");
        goto err;
    }
    if (ehdr->e_ident[EI_CLASS] != ELF_TARG_CLASS || ehdr->e_ident[EI_DATA] != ELF_TARG_DATA)
    {
        _libelf_debug(NONE, "\tunsupported file layout\n");
        goto err;
    }
    if (ehdr->e_ident[EI_VERSION] != EV_CURRENT || ehdr->e_version != EV_CURRENT)
    {
        _libelf_debug(NONE, "\tunsupported file version\n");
        goto err;
    }
#if defined(CONFIG_LIBELF_DYNAMIC)
    if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN)
#else
    if (ehdr->e_type != ET_EXEC)
#endif
    {
        _libelf_debug(NONE, "\tunsupported file type\n");
        goto err;
    }
    if (ehdr->e_machine != ELF_TARG_ARCH)
    {
        _libelf_debug(NONE, "\tunsupported machine\n");
        goto err;
    }

    /* Get the program headers */
    assert(ehdr->e_phentsize == sizeof(elf_phdr_t));
    if ((p = malloc(ehdr->e_phnum*ehdr->e_phentsize)) == NULL)
        goto err;
    

    /* If it's contained in the previous read, let's memcopy it */
    if (ehdr->e_phoff + ehdr->e_phnum*ehdr->e_phentsize <= ELF_HDR_SIZE)
        memcpy(p, buf + ehdr->e_phoff, ehdr->e_phnum*ehdr->e_phentsize);

    /* Otherwise, let's read it from the file */
    else
    {
        if (fseek(file, ehdr->e_phoff, SEEK_SET) != 0)
            goto err_mem;

        if (fread(p, ehdr->e_phentsize, ehdr->e_phnum, file) != ehdr->e_phnum)
            goto err_mem;
    }

    /* we could retrieve phdr later via the text segment, but for DSO, 
     * we don't have the information directly, so let's make that 
     * generic: we keep the allocated phdr and ignore the phdr in text segment */
    elfobj->phdr = p;
    elfobj->phnum = ehdr->e_phnum;
    elfobj->entrypoint = ehdr->e_entry;

    return 0;

err_mem:
    free(p);
err:
    return -1;
}

/* Scan Program Header and get information
 *
 * @param elfobj Dynamic object
 * @return error_t Error code if any
 */
#if defined(CONFIG_LIBELF_DEBUG)
static const char* const _elf_phdr_type_names[8] = {
    "PT_NULL", "PT_LOAD", "PT_DYNAMIC", "PT_INTERP",
    "PT_NOTE", "PT_SHLIB", "PT_PHDR", "PT_TLS"
};
#endif
static error_t _elf_scan_phdr(struct obj_elf_s *elfobj)
{
    size_t i;
    size_t nsegs = 0;

    _libelf_debug(DEBUG, "_elf_scan_phdr\n");

    /* Scan the program header, and save key information. */
    for (i = 0; i < elfobj->phnum; i++)
    {
        elf_phdr_t *phdr_entry = &elfobj->phdr[i];

        if (phdr_entry->p_type < 8)
            _libelf_debug(DEBUG, "\ttype \'%s\'\n", _elf_phdr_type_names[phdr_entry->p_type]);
        else
            _libelf_debug(DEBUG, "\ttype \'0x%x\'\n", phdr_entry->p_type);
        switch (phdr_entry->p_type)
        {
            /* Program header segment */
            case PT_PHDR:
                /* let's skip it and keep our mallocated phdr */
                /* too bad for memory consumption */
                break;

            case PT_INTERP:
                /* It's the only way we can know it's a program and not a shared library that we are loading */
                elfobj->program = 1;
                break;

            /* Loadable segments */
            case PT_LOAD:
                assert(nsegs < 2);
                if (nsegs == 0) /* text segment is first */
                    elfobj->textseg = phdr_entry;
                else
                    elfobj->dataseg = phdr_entry;
                nsegs++;
                break;

            /* TLS segment */
            case PT_TLS:
                elfobj->tlsseg = phdr_entry;
                break;

#if defined(CONFIG_LIBELF_DYNAMIC)
            /* Dynamic segment */
            case PT_DYNAMIC:
                elfobj->dynamic = (const elf_dyn_t*) phdr_entry->p_vaddr;
                break;
#endif

            default:
                _libelf_debug(DEBUG, "\t\tignored entry\n");
                break;
        }
    }

#if defined (CONFIG_LIBELF_DYNAMIC)
    /* If we use the runtime loader, we expect the object to be dynamic in
     * order to relocate it */
    if (elfobj->dynamic == NULL)
    {
        _libelf_debug(NONE, "\terror: loaded object is not dynamically-linked");
        return -1;
    }
#endif

    return 0;
}

/* Load loadable segments from the ELF file
 *
 * @param file File descriptor on the ELF file
 * @param elfobj Dynamic object
 * @return error_t Error code if any
 */
static error_t _elf_load_segments(FILE *file, struct obj_elf_s *elfobj)
{
    _libelf_debug(DEBUG, "_elf_load_segments\n");

    elfobj->vaddrbase  = ALIGN_VALUE_LOW(elfobj->textseg->p_vaddr, elfobj->textseg->p_align);

    size_t text_size = ALIGN_VALUE_UP(elfobj->textseg->p_vaddr + elfobj->textseg->p_memsz, elfobj->textseg->p_align)
        - elfobj->vaddrbase;
    size_t data_size = ALIGN_VALUE_UP(elfobj->dataseg->p_vaddr + elfobj->dataseg->p_memsz, elfobj->dataseg->p_align)
        - ALIGN_VALUE_LOW(elfobj->dataseg->p_vaddr, elfobj->dataseg->p_align);

#if defined (CONFIG_LIBELF_DYNAMIC)
    /* default allocation by malloc */
    size_t size = text_size + data_size;
    /* load in cached memory by default */
    if ((elfobj->mapbase = (uintptr_t)mem_alloc(size, (mem_scope_cpu))) == 0)
    {
        _libelf_debug(NONE, "\tfailed to allocate memory for elf loading\n");
        return -1;
    }
#else
    /* no need to allocate memory, we have to load at the same address that is defined in the elf */
    elfobj->mapbase = elfobj->vaddrbase;
#endif
    elfobj->mapsize = text_size + data_size;

    /*
     * Load the text segment in memory
     */
    _libelf_debug(TRACE, "\tload read-only segments (text, rodata, bss)\n");
    fseek(file, elfobj->textseg->p_offset, SEEK_SET);
    if (fread((void*)elfobj->mapbase, elfobj->textseg->p_filesz, 1, file) != 1) 
    {
        _libelf_debug(NONE, "\t\tcould not read text segment\n");
        goto err_alloc;
    }

    /*
     * Load the data segment in memory
     */
    _libelf_debug(TRACE, "\tload read-write segments (data)\n");
    fseek(file, elfobj->dataseg->p_offset, SEEK_SET);
    if (fread((void*)(elfobj->mapbase + (elfobj->dataseg->p_vaddr - elfobj->textseg->p_vaddr)), 
                        elfobj->dataseg->p_filesz, 1, file) != 1)
    {
        _libelf_debug(NONE, "\t\tcould not read data segment\n");
        goto err_alloc;
    }

    /* clear bss (at the end of data) */
    memset((void*)(elfobj->mapbase + (elfobj->dataseg->p_vaddr - elfobj->textseg->p_vaddr) + elfobj->dataseg->p_filesz), 
                0, elfobj->dataseg->p_memsz - elfobj->dataseg->p_filesz);

#if defined (CONFIG_LIBELF_DYNAMIC)
    /* relocation value */
    elfobj->relocbase = elfobj->mapbase - elfobj->vaddrbase;

    /* relocate some stuffs */
    elfobj->dynamic = (elf_dyn_t*)((elf_addr_t)elfobj->dynamic + elfobj->relocbase);
    if (elfobj->entrypoint != 0)
        elfobj->entrypoint = elfobj->entrypoint + elfobj->relocbase;
#endif

    _libelf_debug(NONE,   "\tobject map:\n");
    _libelf_debug(NONE,   "\t\tmapbase: %p\n",    (void*)elfobj->mapbase);
    _libelf_debug(DEBUG,  "\t\ttextsize: 0x%x\n", text_size);
    _libelf_debug(DEBUG,  "\t\tdatasize: 0x%x\n", data_size);
    _libelf_debug(NONE,   "\t\tvaddrbase: %p\n",  (void*)elfobj->vaddrbase);
#if defined (CONFIG_LIBELF_DYNAMIC)
    _libelf_debug(NONE,   "\t\trelocbase: %p\n",  (void*)elfobj->relocbase);
    _libelf_debug(DEBUG,  "\t\tdynamic: %p\n",    (void*)elfobj->dynamic);
#endif
    _libelf_debug(NONE,   "\t\tentrypoint: %p\n", (void*)elfobj->entrypoint);
    _libelf_debug(DEBUG,  "\t\tphdr: %p\n",       (void*)elfobj->phdr);

    return 0;

err_alloc:
    /* FIXME */
    //free((void*)elfobj->mapbase);
    return -1;
}

/* Process ELF headers (load and scan)
 *
 * @param file File descriptor on the ELF file
 * @param elfobj Dynamic object
 * @return error_t Error code if any
 */
static error_t _elf_process_headers(FILE *file, struct obj_elf_s *elfobj)
{
    _libelf_debug(DEBUG, "elf_process_headers\n");

    if (_elf_load_headers(file, elfobj) != 0)
        goto err;

    if (_elf_scan_phdr(elfobj) != 0)
        goto err_mem;

    return 0;

err_mem:
    free(elfobj->phdr);
err:
    return -1;
}

/* Load ELF file into memory
 *
 * @param pathname Pathname of the ELF file
 * @param elfobj Dynamic object
 * @return error_t Error code if any
 */
error_t elf_load_file(const char *pathname, struct obj_elf_s *elfobj)
{
    char *elfobj_name=pathname;

    FILE* file;

    _libelf_debug(DEBUG, "elf_load_file\n");

    _libelf_debug(TRACE, "\topen file \"%s\"\n", elfobj_name);
    if ((file = fopen(elfobj_name, "r")) == NULL)
    {
        _libelf_debug(NONE, "\t\tcannot open \"%s\"\n", elfobj_name);
        goto err_f;
    }
    elfobj->pathname = strdup(elfobj_name);

    if (_elf_process_headers(file, elfobj) != 0)
        goto err_f;

    if (_elf_load_segments(file, elfobj) != 0)
        goto err_phdr;

    fclose(file);

    return 0;

err_phdr:
    free(elfobj->phdr);
err_f:
    fclose(file);
    return -1;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

