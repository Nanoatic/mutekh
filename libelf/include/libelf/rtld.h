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

/**
 * @file
 * @module{Elf loader library}
 * @short Elf loading with relocation
 */

#ifndef _RTLD_H_
#define _RTLD_H_

#include <hexo/types.h>
#include <hexo/error.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_dlist.h>

#include <libelf/elf.h>
#include <libelf/tls.h>

/*
 * Internal structure
 */

/** @internal rtld object descriptor structure */
struct dynobj_rtld_s
{
    /** elf descriptor structure */
    struct obj_elf_s elf;

    /** number of uses of this object (FIXME: memory management in all libelf is total bullshit) */
    uint_fast32_t   refcount;
    /** indicates wether the object was generated with "-Bsymbolic" */
    bool_t          symbolic;
    /** indicates wether the object has finished being loaded */
    bool_t          relocated;

    /** pointer to the Global Offset Table */
    elf_addr_t  *got;

    /** relocation entries */
    const elf_reloc_t   *rel;
    /** size of relocation info (in bytes) */
    size_t              relsize;
    /** PLT relocation entries */
    const elf_reloc_t   *pltrel;
    /** size of PLT relocation info (in bytes) */
    size_t              pltrelsize;

    /** pointer to the symbol table */
    const elf_sym_t     *symtab;
    /** pointer to the string table */
    const char          *strtab;
    /** size of string table (in bytes) */
    size_t              strsize;

#if defined(CONFIG_CPU_MIPS)
    size_t  mips_gotsym;
    size_t  mips_local_gotno;
    size_t  mips_symtabno;
#endif

    /** hash table buckets array */
    const elf_addr_t    *buckets;
    /** number of buckets */
    reg_t               nbuckets;
    /** hash table chain array */
    const elf_addr_t    *chains;
    /** number of chains */
    reg_t               nchains;

    /** dependency list */
    struct dynobj_rtld_s    **dep_shobj;
    /** number of dependencies */
    size_t                  ndep_shobj;

#if defined(CONFIG_LIBELF_RTLD_TLS)
    /** tls descriptor structure */
    struct dynobj_tls_s tls;
#endif

    /** gpct pointer for double-linked list */
    /* we are forced to have dlist since link order is important */
    CONTAINER_ENTRY_TYPE(DLIST)	list_entry;
};

/** @internal List type for rtld objects */
CONTAINER_TYPE(dynobj_list, DLIST, struct dynobj_rtld_s, list_entry);
CONTAINER_FUNC(dynobj_list, DLIST, static inline, dynobj_list, list_entry);

/* 
 * Scanning callback
 */
/**
 * scan_chain() function template
 * @see rtld_scan_chain_t
 */
#define RTLD_SCAN_CHAIN(n) error_t (n) (const struct dynobj_rtld_s *dynobj, void *priv_data)
/** scan_chain() function type. It allows the user to scan a chain of loaded
 * elf executables (starting from an application) and to perform an action for
 * on each scanned object.
 *
 * @param dynobj currently scanned elf object
 * @param priv_data userprivate data
 * @see #RTLD_SCAN_CHAIN
 */
typedef RTLD_SCAN_CHAIN(rtld_scan_chain_t);

/* 
 * Functions prototypes
 */

/** @this initializes internal lists */
error_t rtld_init (void);

/** @this opens a elf executable
 *
 * @param dynobj pointer on the object descriptor (is allocated in the function)
 * @param pathname pathname of the executable
 * @return error code if any
 */
error_t rtld_open (struct dynobj_rtld_s **dynobj, const char *pathname);

/** @this looks up for a symbol
 *
 * @param dynobj pointer on the object descriptor
 * @param name name of the searched symbol
 * @param sym pointer on the symbol if found (assigned in the function)
 * @return error code if any
 */
error_t rtld_sym (const struct dynobj_rtld_s *dynobj, const char *name, uintptr_t *sym);

/** @this retrieves the size of the required tls segment
 *
 * @param dynobj pointer on the object descriptor
 * @param tls_size size of the required tls (assigned in the function)
 * @return error code if any
 */
error_t rtld_tls_size (const struct dynobj_rtld_s *dynobj, size_t *tls_size);

/** @this inits the tls segment
 *
 * @param dynobj pointer on the object descriptor
 * @param tls pointer on the tls segment
 * @param threadpointer pointer on the tls segment (TP version) (assigned in the function)
 * @return error code if any
 */
error_t rtld_tls_init (const struct dynobj_rtld_s *dynobj, uintptr_t tls, uintptr_t *threadpointer);

/** @this closes the object (not implemented)
 *
 * @param dynobj pointer on the object descriptor
 * @return error code if any
 */
error_t rtld_close (const struct dynobj_rtld_s *dynobj);


/*
 * cpu dependent stuff
 */
#include <cpu/rtld.h>
void _rtld_init_got (const struct dynobj_rtld_s *dynobj);
error_t _rtld_parse_nonplt_relocations(const struct dynobj_rtld_s *dynobj, const struct dynobj_rtld_s *root_dynobj);
error_t _rtld_parse_plt_relocations(const struct dynobj_rtld_s *dynobj, const struct dynobj_rtld_s *root_dynobj);

#endif /* _RTLD_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

