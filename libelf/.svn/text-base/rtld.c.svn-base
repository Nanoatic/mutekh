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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libelf/rtld.h>

static error_t _rtld_partial_load_dynobj(struct dynobj_rtld_s **dynobj, const char *pathname,
        dynobj_list_root_t *list_dynobj, dynobj_list_root_t *list_dep);

/* Compute the elf hash on the given name
 *
 * @param name Name to be hashed
 * @return reg_t Hash of the name
 */
reg_t _rtld_elf_hash(const char *name)
{
    const unsigned char *n = (const unsigned char*)name;
    /*
     * from uClibc
     */
    reg_t hash=0;
    reg_t tmp;

    while (*n)
    {
        hash = (hash << 4) + *n++;
        tmp = hash & 0xf0000000;
        /* The algorithm specified in the ELF ABI is as follows:
           if (tmp != 0)
               hash ^= tmp >> 24;
           hash &= ~tmp;
           But the following is equivalent and a lot
           faster, especially on modern processors. */
        hash ^= tmp;
        hash ^= tmp >> 24;
    }
    return hash;
}

/* Check if a symbol matches with another
 *
 * @param sym The tried symbol
 * @param sym_name The name of the tried symbol
 * @param name Name of the seeked symbol
 * @param type_class Reloc type classes (cpu dependent)
 * @return bool_t Was there a match?
 */
static bool_t
_rtld_check_match_sym(const elf_sym_t *sym, const char *sym_name, const char *name, uint_fast8_t type_class)
{
    _libelf_debug(DEBUG, "_rtld_check_match_sym\n");
    _libelf_debug(DEBUG, "\tchecking match with symbol \"%s\"\n", sym_name);

    /*
     * from uClibc and glibc
     */
    if (type_class & (sym->st_shndx == SHN_UNDEF))
        /* undefined symbol itself */
        return 0;

    if (sym->st_value == 0 && ELF_ST_TYPE(sym->st_info) != STT_TLS)
        /* No value (accepted for TLS, since offset can be null) */
        return 0;

    if (ELF_ST_TYPE(sym->st_info) > STT_FUNC
            && ELF_ST_TYPE(sym->st_info) != STT_COMMON
            && ELF_ST_TYPE(sym->st_info) != STT_TLS)
        /* Ignore all but STT_NOTYPE, STT_OBJECT, STT_FUNC
         * and STT_COMMON entries (and STT_TLS) since these are no
         * code/data definitions
         */
        return 0;

    if (strcmp(sym_name, name) != 0)
        return 0;

    /* This is the matching symbol */
    _libelf_debug(DEBUG, "\t\tmatch!\n");
    return 1;
}

/* Search a symbol by its name within a given dynamic object
 *
 * @param name Name of the seeked symbol
 * @param hash Hash of the name of the seeked symbol (to avoid reperforming the computation)
 * @param dynobj The dynamic object in which the seeked symbol is looking up
 * @param type_class Reloc type classes (cpu dependent)
 * @return elf_sym_t The reference to the symbol if found, NULL otherwise
 */
const elf_sym_t*
_rtld_lookup_sym_dynobj(const struct dynobj_rtld_s *dynobj, const char *name, const reg_t hash, uint_fast8_t type_class)
{
    _libelf_debug(DEBUG, "_rtld_lookup_sym_dynobj\n");

    _libelf_debug(DEBUG, "\tlooking up symbol \"%s\" in object \"%s\"\n", name, dynobj->elf.pathname);

    /* first symbol index in the bucket which matches the hash */
    reg_t sym_idx = dynobj->buckets[hash % dynobj->nbuckets];

    for(; sym_idx != STN_UNDEF; sym_idx = dynobj->chains[sym_idx])
    {
        const elf_sym_t *sym;
        const char *sym_name;

        /* find the symbol in the symbol table */
        assert(sym_idx < dynobj->nchains);
        sym = dynobj->symtab + sym_idx;

        /* find the symbol name */
        assert(sym->st_name != 0);
        sym_name = dynobj->strtab + sym->st_name;

        /* do we have a match on this symbol? */
        if (_rtld_check_match_sym(sym, sym_name, name, type_class) != 0)
            return sym;
    }

    /* nothing was found in this object */
    return NULL;
}

/* Find the definition of a symbol recursively in the relocation chain
 *
 * @param ref_sym Referrenced symbol
 * @param ref_dynobj The dynamic object referrencing the seeked symbol
 * @param def_sym Symbol we found if any
 * @param def_dynobj The dynamic object the found symbol is defined into if any
 * @param def_weak A symbol has been found but is weak
 * @param chain_dynobj The dynamic object we are looking up in
 * @param type_class Reloc type classes (cpu dependent)
 * @return error_t Error code: -1 is not found, 0 is found
 */
static error_t
_rtld_lookup_sym_chain(const elf_sym_t *ref_sym, const struct dynobj_rtld_s *ref_dynobj,
       const elf_sym_t **def_sym, const struct dynobj_rtld_s **def_dynobj, bool_t *def_weak,
       const struct dynobj_rtld_s *chain_dynobj,
       uint_fast8_t type_class)
{
    const elf_sym_t *sym;
    const char *sym_name;
    reg_t sym_hash;

    _libelf_debug(DEBUG, "_rtld_lookup_sym_chain\n");

    sym_name = ref_dynobj->strtab + ref_sym->st_name;
    sym_hash = _rtld_elf_hash(sym_name);

    _libelf_debug(DEBUG, "\tlooking up symbol \"%s\"\n", sym_name);

    /* look up into the chain object:
     *  only if the chain object is not the referrencing object or
     *  if the referrencing object is not symbolic
     */
    if (chain_dynobj != ref_dynobj || !ref_dynobj->symbolic)
    {
        sym = _rtld_lookup_sym_dynobj(chain_dynobj, sym_name, sym_hash, type_class);

        if (sym != NULL)
        {
            /* stop scanning only if strong definition */
            if (ELF_ST_BIND(sym->st_info) != STB_WEAK)
            {
                _libelf_debug(DEBUG, "\t\tfound and strong\n");

                *def_sym = sym;
                *def_dynobj = chain_dynobj;
                *def_weak = 0;

                return 0;
            }
            /* do not override if a previous weak has already been found */
            else if (*def_weak == 0 && ELF_ST_BIND(sym->st_info) == STB_WEAK)
            {
                _libelf_debug(DEBUG, "\t\tfound but weak\n");
                *def_sym = sym;
                *def_dynobj = chain_dynobj;
                *def_weak = 1;
            }
            else
                _libelf_debug(DEBUG, "\t\tfound but previous weak definition\n");
        }
        else
            _libelf_debug(DEBUG, "\t\tnot found\n");
    }

    /* recurse on the dependencies */
    size_t ndep_shobj;
    for (ndep_shobj = 0; ndep_shobj < chain_dynobj->ndep_shobj; ndep_shobj++)
    {
        /* if strong definition found, stop and return. Else, continue */
        if (_rtld_lookup_sym_chain(ref_sym, ref_dynobj,
                    def_sym, def_dynobj, def_weak,
                    chain_dynobj->dep_shobj[ndep_shobj], type_class) == 0
                && *def_weak == 0)
            return 0;
    }

    /* nothing was found */
    if (*def_weak == 0)
        return -1;

    /* a weak definition was found */
    return 0;
}

/* Find the definition of a symbol
 *  1/ in the referrencing object if symbolic
 *  2/ launch the search in the relocation chain the referrencing object belongs to
 *
 * @param ref_sym Referrenced symbol
 * @param ref_dynobj The dynamic object referrencing the seeked symbol
 * @param def_sym Symbol we found if any
 * @param def_dynobj The dynamic object the symbol is defined into if any
 * @param root_dynobj The dynamic object from which the relocation chain begins
 * @param type_class Reloc type classes (cpu dependent)
 * @return error_t Error code if any
 */
error_t
_rtld_lookup_sym(const elf_sym_t *ref_sym, const struct dynobj_rtld_s *ref_dynobj,
       const elf_sym_t **def_sym, const struct dynobj_rtld_s **def_dynobj,
       const struct dynobj_rtld_s *root_dynobj,
       uint_fast8_t type_class)
{
    const elf_sym_t *sym;
    const char *sym_name;
    reg_t sym_hash;

    _libelf_debug(DEBUG, "_rtld_lookup_sym\n");

    sym_name = ref_dynobj->strtab + ref_sym->st_name;
    sym_hash = _rtld_elf_hash(sym_name);

    _libelf_debug(DEBUG, "\tlooking up symbol \"%s\"\n", sym_name);

    /* if the shared dynamic object was compiled with -Bsymbolic,
     * search the global symbol in the referrencing object itself */
    if (ref_dynobj->symbolic)
    {
        _libelf_debug(DEBUG, "\t\tsymbolic flag defined\n");

        sym = _rtld_lookup_sym_dynobj(ref_dynobj, sym_name, sym_hash, type_class);
        if (sym != NULL)
        {
            _libelf_debug(DEBUG, "\t\tfound\n");

            *def_sym = sym;
            *def_dynobj = ref_dynobj;

            return 0;
        }
        _libelf_debug(DEBUG, "\t\tnot found\n");
    }

    /* else search in the chain, starting from the first dynamic object (exec or kernel)
     *  - that why the order of the libs are important in the linkage stage (ld)
     *  - however, without mmu, we cannot support executables which have linked the same
     *    libs but in different order (the first loaded order will be used for all)!
     */
    /* starting condition for the recursive call */
    *def_sym = NULL;
    *def_dynobj = NULL;
    bool_t def_weak = 0;

    if (_rtld_lookup_sym_chain(ref_sym, ref_dynobj, def_sym, def_dynobj, &def_weak, root_dynobj, type_class) != 0)
    {
        _libelf_debug(DEBUG, "\t\tcouldn't find the symbol\n");
        return -1;
    }

    return 0;
}

/* Relocate a given object and its dependencies
 *
 * @param dynobj Object to relocate
 * @param root_dynobj Object which is the root of the relocation chain
 * @param bind_now Allows lazy binding (not implemented though)
 * @return error_t Error code if any
 */
static error_t
_rtld_relocate_dynobj(struct dynobj_rtld_s *dynobj, struct dynobj_rtld_s *root_dynobj, bool_t bind_now)
{
    _libelf_debug(DEBUG, "_rtld_relocate_dynobj\n");

    _libelf_debug(DEBUG, "\trelocate object \"%s\"\n", dynobj->elf.pathname);

    if (dynobj->relocated)
    {
        /* we assume that if the object has already been relocated,
         * its dependencies have been too */
        _libelf_debug(DEBUG, "\t\talready been relocated\n");
        return 0;
    }

    /* sanity check */
    if (dynobj->nbuckets == 0 || dynobj->nchains == 0
            || dynobj->buckets == NULL || dynobj->symtab == NULL
            || dynobj->strtab == NULL)
    {
        _libelf_debug(DEBUG, "\tdynobj has not runtime symbol\n");
        return -1;
    }

    /*
     * First relocate the given object
     */

    /* Relocate non-plt entries */
    if (_rtld_parse_nonplt_relocations(dynobj, root_dynobj) != 0)
        return -1;

    /* Relocate plt entries */
    if (_rtld_parse_plt_relocations(dynobj, root_dynobj) != 0)
        return -1;

    /* set the object as done */
    dynobj->relocated = 1;

    /*
     * Then relocate the dependencies
     */
    size_t ndep_shobj;
    for (ndep_shobj = 0; ndep_shobj < dynobj->ndep_shobj; ndep_shobj++)
    {
#if 1
        /* XXX forbid retro-dependencies:
         * In a shared address space, the data segment which contains the got
         * will also be shared. As a consequence a shared library must remain
         * independent and not link any symbol redefined by applications.
         */
        if (_rtld_relocate_dynobj(dynobj->dep_shobj[ndep_shobj],
                    dynobj->dep_shobj[ndep_shobj],
                    bind_now) != 0)
#else
        /* XXX please note that with virtual memory it is obviously possible
         * (since data segments can be mapped to different pages for each
         * applications).
         */
        if (_rtld_relocate_dynobj(dynobj->dep_shobj[ndep_shobj], root_dynobj, bind_now) != 0)
#endif
            return -1;
    }

    return 0;
}

/* Partially load the dependencies of a given object
 *
 * @param dynobj Object for which load dependencies
 * @param list_dep List of objects to look up into (and to add new object into)
 * @return error_t Error code if any
 */
static error_t
_rtld_load_dependencies(struct dynobj_rtld_s *dynobj,
        dynobj_list_root_t *list_dep)
{
    _libelf_debug(DEBUG, "_rtld_load_dependencies\n");

    const elf_dyn_t *dynp;
    size_t ndep_shobj;

    /* allocate a pointer array to dependencies */
    dynobj->dep_shobj = (struct dynobj_rtld_s**)malloc(dynobj->ndep_shobj*sizeof(struct dynobj_rtld_s*));

    /* second pass on the dynamic section but just for dependencies */
    /* follow the dynamic list but the number of deps is already known */
    for (dynp = dynobj->elf.dynamic, ndep_shobj = 0;
            dynp->d_tag != DT_NULL && ndep_shobj < dynobj->ndep_shobj;
            dynp++)
    {
        switch (dynp->d_tag)
        {
            case DT_NEEDED:
                /* sanity check */
                assert(ndep_shobj < dynobj->ndep_shobj);

                /* find the pathname of the dependency */
                const char *dep_name = dynobj->strtab + dynp->d_un.d_val;
                struct dynobj_rtld_s *dep_dynobj;

                _libelf_debug(DEBUG, "\tload dependency named \"%s\"\n", dep_name);

                /* partially load the dependency into memory */
                if (_rtld_partial_load_dynobj(&dep_dynobj, dep_name, list_dep, list_dep) != 0)
                    return -1;

                _libelf_debug(DEBUG, "\tfinished loading dependency named \"%s\"\n", dep_name);

                /* if the dependency uses TLS */
                if (dep_dynobj->tls.nb_modid)
                {
                    dynobj->tls.nb_modid++;
                    if (dep_dynobj->tls.max_modid > dynobj->tls.max_modid)
                        dynobj->tls.max_modid = dep_dynobj->tls.max_modid;
                    _libelf_debug(DEBUG, "\tdependency has tls; %d tls for object and max modid is %d\n",
                            dynobj->tls.nb_modid, dynobj->tls.max_modid);
                }
                /* add to the dependencies table */
                dynobj->dep_shobj[ndep_shobj] = dep_dynobj;
                ndep_shobj++;
                break;

            default:
                /* nothing */
                break;
        }
    }

    return 0;
}

/* Process the dynamic section and collect information
 *
 * @param dynobj Object to process
 * @return error_t Error code if any
 */
#if defined(CONFIG_LIBELF_DEBUG)
static const char* const _rtld_dyn_tag_names[24] = {
    "DT_NULL", "DT_NEEDED", "DT_PLTRELSZ", "DT_PLTGOT",
    "DT_HASH", "DT_STRTAB", "DT_SYMTAB", "DT_RELA",
    "DT_RELASZ", "DT_RELAENT", "DT_STRSZ", "DT_SYMENT",
    "DT_INIT", "DT_FINI", "DT_SONAME", "DT_RPATH",
    "DT_SYMBOLIC", "DT_REL", "DT_RELSZ", "DT_RELENT",
    "DT_PLTREL", "DT_DEBUG", "DT_TEXTREL", "DT_JMPREL"
};
#endif
static error_t
_rtld_process_dynamic(struct dynobj_rtld_s *dynobj)
{
    _libelf_debug(DEBUG, "_rtld_process_dynamic\n");

    const elf_dyn_t *dynp;

    for (dynp = dynobj->elf.dynamic; dynp->d_tag != DT_NULL; dynp++)
    {
        if (dynp->d_tag < 24)
            _libelf_debug(DEBUG, "\ttag \'%s\':", _rtld_dyn_tag_names[dynp->d_tag]);
        else
            _libelf_debug(DEBUG, "\ttag \'0x%x\':", dynp->d_tag);
        _libelf_debug(DEBUG, " 0x%x\n", dynp->d_un.d_val);

        switch (dynp->d_tag)
        {
            case DT_REL:
            case DT_RELA:
                assert(DT_SUPPORTED_RELOC_TYPE == dynp->d_tag);
                dynobj->rel = (const elf_reloc_t *) (dynobj->elf.relocbase + dynp->d_un.d_ptr);
                break;
            case DT_RELSZ:
            case DT_RELASZ:
                dynobj->relsize = dynp->d_un.d_val;
                break;
            case DT_RELAENT:
            case DT_RELENT:
                assert(dynp->d_un.d_val == sizeof(elf_reloc_t));
                break;
            case DT_JMPREL:
                dynobj->pltrel = (const elf_reloc_t *)(dynobj->elf.relocbase + dynp->d_un.d_ptr);
                break;
            case DT_PLTRELSZ:
                dynobj->pltrelsize = dynp->d_un.d_val;
                break;
            case DT_PLTREL:
                assert(DT_SUPPORTED_RELOC_TYPE == dynp->d_un.d_val);
                break;
            case DT_SYMTAB:
                dynobj->symtab = (const elf_sym_t *)(dynobj->elf.relocbase + dynp->d_un.d_ptr);
                break;
            case DT_SYMENT:
                assert(dynp->d_un.d_val == sizeof(elf_sym_t));
                break;
            case DT_STRTAB:
                dynobj->strtab = (const char *)(dynobj->elf.relocbase + dynp->d_un.d_ptr);
                break;
            case DT_STRSZ:
                dynobj->strsize = dynp->d_un.d_val;
                break;
            case DT_HASH:
                {
                    const elf_addr_t *hashtab = (const elf_addr_t *)(dynobj->elf.relocbase + dynp->d_un.d_ptr);
                    dynobj->nbuckets = hashtab[0];
                    dynobj->nchains = hashtab[1];
                    dynobj->buckets = &hashtab[2];
                    dynobj->chains = dynobj->buckets + dynobj->nbuckets;
                }
                break;
            case DT_NEEDED:
                _libelf_debug(DEBUG, "\t\tNeeded library handled later...\n");
                dynobj->ndep_shobj++;
                break;
            case DT_PLTGOT:
                dynobj->got = (elf_addr_t *)(dynobj->elf.relocbase + dynp->d_un.d_ptr);
                break;

            case DT_TEXTREL:
                _libelf_debug(NONE, "\t\tnot supported!\n");
                return -1;

            case DT_SYMBOLIC:
                dynobj->symbolic = 1;
                break;

            case DT_RPATH:
                _libelf_debug(NONE, "\t\tDT_RPATH ignored:\n\
                        \t\t- all needed libs must be in the program directory\n\
                        \t\t- still continuing but it may fail...\n");
                break;

            case DT_SONAME:
                _libelf_debug(DEBUG, "\t\tignored!\n");
                break;

            case DT_INIT:
                //dynobj->init = (void (*)(void))(dynobj->elf.relocbase + dynp->d_un.d_ptr);
                _libelf_debug(NONE, "\t\tDT_INIT ignored!\n");
                break;

            case DT_FINI:
                //dynobj->fini = (void (*)(void))(dynobj->elf.relocbase + dynp->d_un.d_ptr);
                _libelf_debug(NONE, "\t\tDT_FINI ignored!\n");
                break;

            case DT_DEBUG:
                _libelf_debug(DEBUG, "\t\tignored!\n");
                break;

            case DT_FLAGS:
                if (dynp->d_un.d_val & DF_STATIC_TLS)
                {
                    if (dynobj->elf.program)
                        _libelf_debug(DEBUG, "\t\tStatic model for TLS is used for this executable\n");
                    else
                    {
                        _libelf_debug(NONE, "\t\tStatic model for TLS is not supported for shared libs!\n");
                        return -1;
                    }
                }
                break;

#if defined(CONFIG_CPU_MIPS)
            case DT_MIPS_GOTSYM:
                dynobj->mips_gotsym = dynp->d_un.d_val;
                _libelf_debug(DEBUG, "\t\tmips tag DT_MIPS_GOTSYM\n");
                break;
            case DT_MIPS_LOCAL_GOTNO:
                dynobj->mips_local_gotno = dynp->d_un.d_val;
                _libelf_debug(DEBUG, "\t\tmips tag DT_MIPS_LOCAL_GOTNO\n");
                break;
            case DT_MIPS_SYMTABNO:
                dynobj->mips_symtabno = dynp->d_un.d_val;
                _libelf_debug(DEBUG, "\t\tmips tag DT_MIPS_SYMTABNO\n");
                break;
#endif

            default:
                _libelf_debug(DEBUG, "\t\tignored\n");
                break;
        }
    }

    if (dynobj->got)
        /* relocate stub addr
         * (in case of lazy binding - not supported here) */
        _rtld_init_got(dynobj);

    return 0;
}

#if 0 /* Not used yet */
/* Look up for a dynamic object from an address
 *
 * @param addr The address at which we look up
 * @param list_lookup List of objects to look up into
 * @return struct dynobj_rtld_s Return the found object if any
 */
static const struct dynobj_rtld_s*
_rtld_lookup_addr(const uintptr_t addr, dynobj_list_root_t *list)
{
#define END_SYM (char*)"_end"

    _libelf_debug(DEBUG, "_rtld_lookup_addr\n");
    _libelf_debug(DEBUG, "\tlook up for dynobj at addr %0x\n", addr);

    CONTAINER_FOREACH(dynobj_list, DLIST, list,
    {
        const elf_sym_t *end_sym;

        if (addr < item->elf.mapbase)
            CONTAINER_FOREACH_CONTINUE;
        if ((end_sym = _rtld_lookup_sym_dynobj(END_SYM, _rtld_elf_hash(END_SYM), item, ELF_RTYPE_CLASS_PLT)) != NULL)
        {
            if (addr < (item->elf.relocbase + end_sym->st_value))
            {
                _libelf_debug(DEBUG, "\tfound (\"%s\")\n", item->elf.pathname);
                return item;
            }
        }
    });

    _libelf_debug(DEBUG, "\tnot found\n");
    return NULL;

#undef END_SYM
}
#endif

/* Look up for a dynamic object from its reference
 *
 * @param dynobj Reference of the object
 * @param list_lookup List of objects to look up into
 * @return struct dynobj_rtld_s Return the found object if any
 */
struct dynobj_rtld_s*
_rtld_lookup_ref(const struct dynobj_rtld_s *dynobj,
        dynobj_list_root_t *list_lookup)
{
    _libelf_debug(DEBUG, "_rtld_lookup_ref\n");

    CONTAINER_FOREACH(dynobj_list, DLIST, list_lookup,
    {
        if (dynobj == item)
        {
            _libelf_debug(DEBUG, "\tfound\n");
            return item;
        }
    });

    _libelf_debug(DEBUG, "\tnot found\n");
    return NULL;
}

/* Look up for a dynamic object from its name into a given list
 *
 * @param pathname Pathname of the object file
 * @param list_lookup List of objects to look up into
 * @return struct dynobj_rtld_s Return the found object if any
 */
static struct dynobj_rtld_s*
_rtld_lookup_name(const char *pathname,
        dynobj_list_root_t *list_lookup)
{
    const char *dynobj_name=pathname;

    _libelf_debug(DEBUG, "_rtld_lookup_name\n");
    _libelf_debug(DEBUG, "\tlooking up for \"%s\" object\n", dynobj_name);

    CONTAINER_FOREACH(dynobj_list, DLIST, list_lookup,
    {
        if (strcmp(dynobj_name, item->elf.pathname) == 0)
        {
            _libelf_debug(DEBUG, "\tfound\n");
            return item;
        }
    });

    _libelf_debug(DEBUG, "\tnot found\n");
    return NULL;
}

static error_t
_rtld_partial_load_dynobj(struct dynobj_rtld_s **dynobj, const char *pathname, 
        dynobj_list_root_t *list_prgs, dynobj_list_root_t *list_deps)
{
    _libelf_debug(DEBUG, "_rtld_partial_load_dynobj\n");

    /* Load partially a dynamic object and its dependencies
     *  1/ the object is already loaded, do nothing
     *  2/ otherwise, load the object and its dependencies but do not relocate
     */
    struct dynobj_rtld_s *new_dynobj;

	/* first, look up by name in already loaded programs */
	new_dynobj = _rtld_lookup_name(pathname, list_prgs);

	/* if not found, instantiate it completely */
	if (new_dynobj == NULL)
    {
        /* create it */
        new_dynobj = (struct dynobj_rtld_s*)calloc(1, sizeof(struct dynobj_rtld_s));

        if (elf_load_file(pathname, &new_dynobj->elf) != 0)
            goto err;

#if defined (CONFIG_LIBELF_RTLD_TLS)
        if (new_dynobj->elf.tlsseg)
            _tls_set_modid(new_dynobj);
#endif

        /* add the new dynobj at the end of add list */
        dynobj_list_pushback(list_prgs, new_dynobj);

        if (_rtld_process_dynamic(new_dynobj) != 0)
            goto err;

        /* Load any needed shared objects */
        if (_rtld_load_dependencies(new_dynobj, list_deps) != 0)
        {
            _libelf_debug(DEBUG, "\tcould not load dependencies\n");
            goto err_mem;
        }
    }

    new_dynobj->refcount++;
    *dynobj = new_dynobj;

    return 0;

err_mem:
    free(new_dynobj->elf.pathname);
err:
    free(new_dynobj);
    return -1;
}

void _rtld_scan_chain (const struct dynobj_rtld_s *dynobj, rtld_scan_chain_t *fcn, void *data)
{
    fcn(dynobj, data);
    /* recurse for the dependencies */
    size_t ndep_shobj;
    for (ndep_shobj = 0; ndep_shobj < dynobj->ndep_shobj; ndep_shobj++) {
        _rtld_scan_chain(dynobj->dep_shobj[ndep_shobj], fcn, data);
    }
}

error_t _rtld_load_dynobj(struct dynobj_rtld_s **dynobj, const char *pathname,
        dynobj_list_root_t *list_prgs, dynobj_list_root_t *list_deps)
{
    _libelf_debug(DEBUG, "_rtld_load_dynobj\n");

    /* find the object (either by look up or by loading it) */
    if (_rtld_partial_load_dynobj(dynobj, pathname, list_prgs, list_deps) != 0) {
        _libelf_debug(DEBUG, "\tcould not load \"%s\"\n", pathname);
        goto err;
    }
    /* tls management */
    if (_tls_load_dynobj(*dynobj) != 0) {
        _libelf_debug(DEBUG, "\tcould not load tls for \"%s\"\n", pathname);
        goto err;
    }
    /* relocate the object */
    if (_rtld_relocate_dynobj(*dynobj, *dynobj, 1) != 0) {
        _libelf_debug(DEBUG, "\tcould not relocate \"%s\"\n", pathname);
        goto err;
    }

    return 0;
err:
    return -1;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

