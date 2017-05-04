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

    Based on uClibc
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libelf/rtld.h>

/* Private prototype */
error_t _rtld_lookup_sym(const elf_sym_t *ref_sym, const struct dynobj_rtld_s *ref_dynobj,
        const elf_sym_t **def_sym, const struct dynobj_rtld_s **def_dynobj,
        const struct dynobj_rtld_s *root_dynobj,
        uint_fast8_t type_class);

/* Init the stub addresses
 *
 * @param dynobj Object to relocate
 */
void
_rtld_init_got (const struct dynobj_rtld_s *dynobj)
{
#if 0
    dynobj->got[0] = (Elf_Addr) _rtld_bind_start;
    dynobj->got[1] = (Elf_Addr) dynobj;
#endif
}

/* Perform the non plt relocations in the given object
 *
 * @param dynobj Object to relocate
 * @param root_dynobj Object which is the root of the relocation chain
 * @return error_t Error code if any
 */
error_t
_rtld_parse_nonplt_relocations(const struct dynobj_rtld_s *dynobj, const struct dynobj_rtld_s *root_dynobj)
{
    const elf_reloc_t *rel = dynobj->rel;
    const size_t relsize = dynobj->relsize;

    const elf_reloc_t *rel_entry;
    const elf_reloc_t *rel_end;

    _libelf_debug("_rtld_parse_relocations\n");

    /* relsize is in bytes */
	rel_end = (const elf_reloc_t*)((const char*)rel + relsize);

	for (rel_entry = rel; rel_entry < rel_end; rel_entry++)
	{
        elf_addr_t *where;
        reg_t reloc_type;

		const elf_sym_t *ref_sym;
		const elf_sym_t *def_sym;
		const struct dynobj_rtld_s *def_dynobj;

        ref_sym = dynobj->symtab + ELF_R_SYM(rel_entry->r_info);

		_libelf_debug("\trelocate symbol \"%s\": ", dynobj->strtab + ref_sym->st_name);

		where = (elf_addr_t*) (dynobj->elf.relocbase + rel_entry->r_offset);
        reloc_type = ELF_R_TYPE(rel_entry->r_info);

		switch (reloc_type)
        {
            case R_386_NONE:
                _libelf_debug("(R_386_NONE)\n");
                break;

            case R_386_32:
                {
                    if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
                        return -1;

                    _libelf_debug("(R_386_32) from @%p", *where);
                    *where += (elf_addr_t) (def_dynobj->elf.relocbase + def_sym->st_value);
                    _libelf_debug(" to @%p\n", *where);
                }
                break;

            case R_386_PC32:
                /*
                 * I don't think the dynamic linker should ever see this
                 * type of relocation.  But the binutils-2.6 tools sometimes
                 * generate it.
                 */
                {
                    if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
                        return -1;

                    _libelf_debug("(R_386_PC32) from @%p", *where);
                    *where += (elf_addr_t)(def_dynobj->elf.relocbase + def_sym->st_value) - (elf_addr_t)where;
                    _libelf_debug(" to @%p\n", *where);
                }
                break;

            case R_386_GLOB_DAT:
                {
                    if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
                        return -1;

                    *where = (elf_addr_t)(def_dynobj->elf.relocbase + def_sym->st_value);
                    _libelf_debug("(R_386_GLOB_DAT) to @%p\n", *where);
                }
                break;

            case R_386_RELATIVE:
                {
                    _libelf_debug("(R_386_RELATIVE) from @%p", *where);
                    *where += (elf_addr_t)(dynobj->elf.relocbase);
                    _libelf_debug(" to @%p\n", *where);
                }
                break;

            case R_386_COPY:
                {
                    /* only authorised in programs */
                    assert(dynobj->program);

                    if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
                        return -1;

                    _libelf_debug("(R_386_COPY) from @%p to @%p (%d bytes)\n", (def_dynobj->elf.relocbase + def_sym->st_value),
                            where, ref_sym->st_size);
                    memcpy(where, (def_dynobj->elf.relocbase + def_sym->st_value), ref_sym->st_size);
                }
                break;

            default:
                _libelf_debug("\tunsupported relocation type %d\n", reloc_type);
                return -1;
        }
    }

    return 0;
}

/* Perform the plt relocations in the given object
 *
 * @param dynobj Object to relocate
 * @param root_dynobj Object which is the root of the relocation chain
 * @return error_t Error code if any
 */
error_t
_rtld_parse_plt_relocations(const struct dynobj_rtld_s *dynobj, const struct dynobj_rtld_s *root_dynobj)
{
    const elf_reloc_t *rel = dynobj->pltrel;
    const size_t relsize = dynobj->pltrelsize;

	const elf_reloc_t *rel_entry;
	const elf_reloc_t *rel_end;

    _libelf_debug("_rtld_parse_relocations\n");

    /* relsize is in bytes */
	rel_end = (const elf_reloc_t*)((const char*)rel + relsize);

	for (rel_entry = rel; rel_entry < rel_end; rel_entry++)
	{
        elf_addr_t *where;

		const elf_sym_t *ref_sym;
		const elf_sym_t *def_sym;
		const struct dynobj_rtld_s *def_dynobj;

        ref_sym = dynobj->symtab + ELF_R_SYM(rel_entry->r_info);

		_libelf_debug("\trelocate symbol \"%s\": ", dynobj->strtab + ref_sym->st_name);

		where = (elf_addr_t*) (dynobj->elf.relocbase + rel_entry->r_offset);

        assert(ELF_R_TYPE(rel_entry->r_info) == R_386_JMP_SLOT);

        if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
            return -1;

        *where = (elf_addr_t)(def_dynobj->elf.relocbase + def_sym->st_value);
        _libelf_debug("(R_386_JMP_SLOT) to @%p\n", *where);
    }

    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

