/* mips/mipsel ELF shared library loader suppport
 *
 * Copyright (C) 2002, Steven J. Hill (sjhill@realitydiluted.com)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the above contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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

   Based on uClibc, and glibc
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
#if 0 /* we do not support lazy binding */
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
	_libelf_debug(DEBUG, "_rtld_parse_nonplt_relocations\n");

    /* first, local entries */
    /* Add load address displacement to all local GOT entries */
    _libelf_debug(DEBUG, "\tlocal GOT entries\n");
    uint32_t idx;
    idx = 2;
    while (idx < dynobj->mips_local_gotno)
        dynobj->got[idx++] += (elf_addr_t) dynobj->elf.relocbase;

    _libelf_debug(DEBUG, "\tglobal GOT entries\n");

	/* goto global entries, skip local entries */
	size_t sym_idx;
	elf_addr_t *got_entry;

	got_entry = dynobj->got + dynobj->mips_local_gotno;

	/* loop through all the mapped symbol */
	for (sym_idx = dynobj->mips_gotsym; sym_idx < dynobj->mips_symtabno; sym_idx++, got_entry++)
	{
		const elf_sym_t *ref_sym;
		const elf_sym_t *def_sym;
		const struct dynobj_rtld_s *def_dynobj;

		ref_sym = dynobj->symtab + sym_idx;

		_libelf_debug(DEBUG, "\trelocate global symbol \"%s\"\n", dynobj->strtab + ref_sym->st_name);

		if (ref_sym->st_shndx == SHN_UNDEF)
		{
            if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, ELF_RTYPE_CLASS_PLT) != 0)
                return -1;

            _libelf_debug(DEBUG, "\trelocate symbol \"%s\": ", dynobj->strtab + ref_sym->st_name);
            _libelf_debug(DEBUG, "from %p", (void*)*got_entry);
            *got_entry = def_sym->st_value + def_dynobj->elf.relocbase;
            _libelf_debug(DEBUG, " to %p\n", (void*)*got_entry);
		}
		else if (ref_sym->st_shndx == SHN_COMMON)
        {
			if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, ELF_RTYPE_CLASS_PLT) != 0)
				return -1;

			_libelf_debug(DEBUG, "\trelocate symbol \"%s\": ", dynobj->strtab + ref_sym->st_name);
			_libelf_debug(DEBUG, "from %p", (void*)*got_entry);
			*got_entry = def_sym->st_value + def_dynobj->elf.relocbase;
			_libelf_debug(DEBUG, " to %p\n", (void*)*got_entry);
		}
		else if (ELF_ST_TYPE(ref_sym->st_info) == STT_SECTION)
        {
			if (ref_sym->st_other == 0)
            {
				_libelf_debug(DEBUG, "\trelocate symbol \"%s\": ", dynobj->strtab + ref_sym->st_name);
                _libelf_debug(DEBUG, "from %p", (void*)*got_entry);
				*got_entry += dynobj->elf.relocbase;
                _libelf_debug(DEBUG, " to %p\n", (void*)*got_entry);
            }
		}
        else {
			if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, ELF_RTYPE_CLASS_PLT) != 0)
				return -1;

			_libelf_debug(DEBUG, "\trelocate symbol \"%s\": ", dynobj->strtab + ref_sym->st_name);
			_libelf_debug(DEBUG, "from %p", (void*)*got_entry);
			*got_entry = def_sym->st_value + def_dynobj->elf.relocbase;
			_libelf_debug(DEBUG, " to %p\n", (void*)*got_entry);
		}
	}

    /* non plt relocations */
    _libelf_debug(DEBUG, "\tnon-plt relocations\n");
    const elf_reloc_t *rel = dynobj->rel;
    const size_t relsize = dynobj->relsize;

	const elf_reloc_t *rel_entry;
	const elf_reloc_t *rel_end;

    /* relsize is in bytes */
	rel_end = (const elf_reloc_t*)((const char*)rel + relsize);

	for (rel_entry = rel; rel_entry < rel_end; rel_entry++)
	{
        elf_addr_t *where;
        reg_t reloc_type;

        size_t sym_index;
		const elf_sym_t *ref_sym;
		const elf_sym_t *def_sym;
		const struct dynobj_rtld_s *def_dynobj;

        sym_index = ELF_R_SYM(rel_entry->r_info);
        ref_sym = dynobj->symtab + sym_index;

		_libelf_debug(DEBUG, "\trelocate symbol \"%s\": ", dynobj->strtab + ref_sym->st_name);

		where = (elf_addr_t*) (dynobj->elf.relocbase + rel_entry->r_offset);
        reloc_type = ELF_R_TYPE(rel_entry->r_info);

		switch (reloc_type)
        {
            case R_MIPS_NONE:
                _libelf_debug(DEBUG, "(R_MIPS_NONE)\n");
                break;

            case R_MIPS_REL32:
                _libelf_debug(DEBUG, "(R_MIPS_REL32)\n\
                        \tfrom %p", (void*)*where);
                /* Please, someone who understood
                 * - either the Mips ABI or the code in usual libc -
                 * explains it to me !!!
                 */
                if (sym_index >= dynobj->mips_gotsym)
                    /* - (*where) contains the address of the relocation
                     * computed by ld at link time.
                     * - got[] has just got being relocated above and contains
                     *   the correct address.
                     * -> how (*where)+got[] can give a good result ?!?!?!
                     */
                    //*where += dynobj->got[dynobj->mips_local_gotno + (sym_index - dynobj->mips_gotsym)];
                    *where = dynobj->got[dynobj->mips_local_gotno + (sym_index - dynobj->mips_gotsym)];
                else
                {
                    /* almost same comment here. (*where) contains what ld
                     * computed at link time. The symbol is not in the got
                     * (must be a local static symbol or something like that)
                     * so it is relocated merely by adding the relocbase. How
                     * adding ref_sym->st_value (which contains the same value
                     * as *where I think) can give a good result ?!?!?!
                     */
                    //if (ref_sym->st_info == ELF_ST_INFO(STB_LOCAL, STT_SECTION))
                    //    *where += ref_sym->st_value;
                    *where += dynobj->elf.relocbase;
                }
                _libelf_debug(DEBUG, " to %p\n", (void*)*where);
                break;

#if defined (CONFIG_LIBELF_RTLD_TLS)
            /* TLS management */
            case R_MIPS_TLS_DTPMOD32:
                {
                    _libelf_debug(DEBUG, "(R_MIPS_TLS_DTPMOD32)\n");
                    /* according to mips guy on gcc-ml, when sym_index is null, it means we have a local-dynamic variable,
                     * the variable comes then from the local object. Otherwise, let's search it in the chain.
                     */
                    if (sym_index)
                    {
                        if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
                            return -1;

                        _libelf_debug(DEBUG, "\tfrom %d", *where);
                        *where = (elf_addr_t) (def_dynobj->tls.modid);
                        _libelf_debug(DEBUG, " to %d\n", *where);
                    } else {
                        _libelf_debug(DEBUG, "\tfrom %d", *where);
                        *where = (elf_addr_t) (dynobj->tls.modid);
                        _libelf_debug(DEBUG, " to %d\n", *where);
                    }
                }
                break;

            case R_MIPS_TLS_DTPREL32:
                {
                    _libelf_debug(DEBUG, "(R_MIPS_TLS_DTPREL32)\n");
                    if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
                        return -1;

                    _libelf_debug(DEBUG, "from %p", (void*)*where);
                    *where = (elf_addr_t) (def_sym->st_value - TLS_DTP_OFFSET);
                    _libelf_debug(DEBUG, " to %p\n", (void*)*where);
                }
                break;

            case R_MIPS_TLS_TPREL32:
                {
                    _libelf_debug(DEBUG, "(R_MIPS_TLS_TPREL32)\n");
                    if (_rtld_lookup_sym(ref_sym, dynobj, &def_sym, &def_dynobj, root_dynobj, elf_machine_type_class(reloc_type)) != 0)
                        return -1;

                    _libelf_debug(DEBUG, "from %p", (void*)*where);
                    *where = (elf_addr_t) (root_dynobj->tls.offset_shobj[def_dynobj->tls.modid] + def_sym->st_value - TLS_TP_OFFSET);
                    _libelf_debug(DEBUG, " to %p\n", (void*)*where);
                }
                break;
#endif

            default:
                _libelf_debug(DEBUG, "unsupported relocation type %d\n", reloc_type);
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
    _libelf_debug(DEBUG, "_rtld_parse_plt_relocations\n");

    /* nothing to do... */

    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

