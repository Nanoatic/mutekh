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

static dynobj_list_root_t prgs_root;    /* List of user programs */
static dynobj_list_root_t deps_root;    /* List of dependencies (shared libs) */

/* Private RTLD prototypes */
error_t _rtld_load_dynobj(struct dynobj_rtld_s **dynobj, const char *pathname, dynobj_list_root_t *list_prg, dynobj_list_root_t *list_dep);
const elf_sym_t* _rtld_lookup_sym_dynobj(const struct dynobj_rtld_s *dynobj, const char *name, const reg_t hash, uint_fast8_t type_class);
const struct dynobj_rtld_s* _rtld_lookup_ref(const struct dynobj_rtld_s *dynobj, dynobj_list_root_t *list_lookup);
void _rtld_scan_chain (const struct dynobj_rtld_s *dynobj, rtld_scan_chain_t *fcn, void *data);

reg_t _rtld_elf_hash(const char *name);


error_t rtld_init (void)
{
	_libelf_debug(DEBUG, "rtld_init\n");
	dynobj_list_init(&prgs_root);
	dynobj_list_init(&deps_root);
    return 0;
}

error_t rtld_open (struct dynobj_rtld_s **dynobj, const char *pathname)
{
	_libelf_debug(DEBUG, "rtld_open\n");

    struct dynobj_rtld_s *new_dynobj;
	if (_rtld_load_dynobj(&new_dynobj, pathname, &prgs_root, &deps_root) != 0)
		return -1;
    *dynobj = new_dynobj;
    return 0;
}

error_t rtld_sym (const struct dynobj_rtld_s *dynobj, const char *name, uintptr_t *sym)
{
	_libelf_debug(DEBUG, "rtld_sym\n");

	const elf_sym_t *dlsym;

	/* allows to access only loaded programs */
	if (_rtld_lookup_ref(dynobj, &prgs_root) == NULL)
		return -1;
	/* look up the symbol in the program 
     * (note that you should link your program with --export-dynamic 
     * if you want to access unreferenced symbol) 
     */
	if ((dlsym = _rtld_lookup_sym_dynobj(dynobj, name, _rtld_elf_hash(name), ELF_RTYPE_CLASS_PLT)) == NULL)
		return -1;
	/* relocate the symbol */
	*sym = (uintptr_t)(dlsym->st_value + dynobj->elf.relocbase);
	return 0;
}

error_t rtld_tls_size (const struct dynobj_rtld_s *dynobj, size_t *tls_size)
{
	_libelf_debug(DEBUG, "rtld_tls_size\n");

	/* allows to access only loaded programs */
	if (_rtld_lookup_ref(dynobj, &prgs_root) == NULL)
		return -1;

    _tls_dynobj_size(dynobj, tls_size);
	return 0;
}

error_t rtld_tls_init (const struct dynobj_rtld_s *dynobj, uintptr_t tls, uintptr_t *threadpointer)
{
	_libelf_debug(DEBUG, "rtld_tls_init\n");

	/* allows to access only loaded programs */
	if (_rtld_lookup_ref(dynobj, &prgs_root) == NULL)
		return -1;
    if (_tls_init_dynobj(dynobj, tls, threadpointer) != 0)
        return -1;
	return 0;
}

error_t rtld_scan_chain (const struct dynobj_rtld_s *dynobj, rtld_scan_chain_t *fcn, void *data)
{
	_libelf_debug(DEBUG, "rtld_scan_chain\n");

	/* allows to access only loaded programs */
	if (_rtld_lookup_ref(dynobj, &prgs_root) == NULL)
		return -1;

    _rtld_scan_chain(dynobj, fcn, data);
	return 0;
}

error_t rtld_close (const struct dynobj_rtld_s *dynobj)
{
    //TODO
	_libelf_debug(DEBUG, "rtld_user_dlclose\n");
    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

