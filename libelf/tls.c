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

/* Return a new modid 
 */
void _tls_set_modid(struct dynobj_rtld_s *dynobj)
{
    /* FIXME: for now, no gaps management */

    /* modid for shared library begins necessarily at 2:
     * 1 is always reserved for the program itself */
    static size_t tls_modid = 2; 

    /* program always get the modid #1 */
    /* otherwise, we get a new modid */
    dynobj->tls.modid = (dynobj->elf.program) ? 1 : tls_modid++;
    dynobj->tls.max_modid = dynobj->tls.modid;

    /* list of dependencies is still unknown */
    dynobj->tls.nb_modid = 1;

    _libelf_debug(DEBUG, "\t\tmodid = %d\n", dynobj->tls.modid);
}

/* Recursively compute offset value for each module in the chain
 * and by side-effect, compute the total size of the tls area
 *
 * @param dynobj Dynamic object
 * @param root_dynobj Program object (head of the chain)
 * @return void 
 */
static void
_tls_compute_offsets(struct dynobj_rtld_s *dynobj, struct dynobj_rtld_s *root_dynobj)
{
    _libelf_debug(DEBUG, "_tls_compute_offsets\n");

    size_t tls_offset;

    /* if the dynobj has a TLS segment, compute the offset */
    if (dynobj->tls.modid != 0)
    {
#if defined(TLS_DTVP_AT_TP)
        tls_offset = ALIGN_VALUE_UP(root_dynobj->tls.total_size, dynobj->elf.tlsseg->p_align);
#elif defined(TLS_TCB_AT_TP)
        tls_offset = ALIGN_VALUE_UP(root_dynobj->tls.total_size + dynobj->elf.tlsseg->p_memsz, 
                dynobj->elf.tlsseg->p_align);
#endif
        root_dynobj->tls.offset_shobj[dynobj->tls.modid] = tls_offset;
        _libelf_debug(DEBUG, "\toffset[\"%s\"-%d] = 0x%x\n", dynobj->elf.pathname, dynobj->tls.modid, tls_offset);

#if defined(TLS_DTVP_AT_TP)
        root_dynobj->tls.total_size = tls_offset + dynobj->elf.tlsseg->p_memsz;
#elif defined(TLS_TCB_AT_TP)
        root_dynobj->tls.total_size = tls_offset;
#endif
        _libelf_debug(DEBUG, "\ttls total size is now: 0x%x\n", root_dynobj->tls.total_size);
    }
    else
        _libelf_debug(DEBUG, "\tno tls offset to compute for \"%s\"\n", dynobj->elf.pathname);

    /*
     * Recurse into the deps
     */
    size_t ndep_shobj;
    for (ndep_shobj = 0; ndep_shobj < dynobj->ndep_shobj; ndep_shobj++)
        /* recurse only if the dep or its deps have tls */
        if (dynobj->dep_shobj[ndep_shobj]->tls.nb_modid)
            _tls_compute_offsets(dynobj->dep_shobj[ndep_shobj], root_dynobj);
}

/* Recursively load tls image in tls area and set DTV fields accordingly
 *
 * @param dynobj Dynamic object
 * @param root_dynobj Program object (head of the chain)
 * @param tp Thread Pointer
 * @param dtv DTV (Dynamic thread vector)
 * @return error_t Error code if any 
 */
static error_t
_tls_load_images(const struct dynobj_rtld_s *dynobj, const struct dynobj_rtld_s *root_dynobj, uintptr_t tp, tls_dtv_t *dtv)
{
    FILE *file;
        
    _libelf_debug(DEBUG, "_tls_load_images\n");
    assert(dynobj->tls.modid <= root_dynobj->tls.max_modid);

    /* if the dynobj has a TLS segment, load its image */
    if (dynobj->tls.modid != 0)
    {
        uintptr_t tls_data;
        /* go to the beginning of the tls data of this module */
#if defined(TLS_DTVP_AT_TP)
        tls_data = tp + root_dynobj->tls.offset_shobj[dynobj->tls.modid];
#elif defined(TLS_TCB_AT_TP)
        tls_data = tp - root_dynobj->tls.offset_shobj[dynobj->tls.modid];
#endif
        
        /* place the file at the tls image */
        _libelf_debug(TRACE, "\topen file \"%s\"\n", dynobj->elf.pathname);
        file = fopen(dynobj->elf.pathname, "r"); /* skip error test, we already succeeded to open it */
        fseek(file, dynobj->elf.tlsseg->p_offset, SEEK_SET);

        /* fill the area */
        if (fread((void*)tls_data, dynobj->elf.tlsseg->p_filesz, 1, file) != 1)
        {
            _libelf_debug(NONE, "\t\tcould not read tls segment");
            goto err_f;
        }
        /* we already zeroed bss with the calloc. I still let this if later we notice that it's better 
         * to just malloc and zero the tbss areas: for now i don't know */
        /* memset((void*)(tls_data + dynobj->elf.tlsseg->p_filesz),
                0, dynobj->elf.tlsseg->p_memsz - dynobj->elf.tlsseg->p_filesz);*/
                

        fclose(file);

        /* Update DTV for this module (with offset) */
        dtv[dynobj->tls.modid].ptr = (void*)tls_data + TLS_DTP_OFFSET;
    }

    /*
     * Recurse into the deps
     */
    size_t ndep_shobj;
    for (ndep_shobj = 0; ndep_shobj < dynobj->ndep_shobj; ndep_shobj++)
        /* recurse only if the dep or its deps have tls */
        if (dynobj->dep_shobj[ndep_shobj]->tls.nb_modid)
            _tls_load_images(dynobj->dep_shobj[ndep_shobj], root_dynobj, tp, dtv);

    return 0;

err_f:
    fclose(file);
    return -1;
}

/* Precompute the offset values for tls area
 *
 * @param dynobj Dynamic object
 * @return error_t Error code if any 
 */
error_t
_tls_load_dynobj(struct dynobj_rtld_s *dynobj)
{
    _libelf_debug(DEBUG, "_tls_load_dynobj\n");

    /* sanity check */
    assert(dynobj->tls.modid <= 1);
    if (dynobj->tls.nb_modid == 0)
    {
        _libelf_debug(DEBUG, "\tno tls for this program\n");
        return 0;
    }

    /* assume that if the object has been relocated, the tls computation has already been done */
    if (dynobj->relocated)
    {
        _libelf_debug(DEBUG, "\ttls management already done\n");
    }
    else {
        _libelf_debug(DEBUG, "\tcreate a list of %d potential offsets to compute\n", dynobj->tls.max_modid + 1);

        /* 
         * compute the offset values for the dependency list 
         */
        dynobj->tls.offset_shobj = (size_t*)calloc(dynobj->tls.max_modid + 1, sizeof(size_t));

#if defined(TLS_DTVP_AT_TP)
        /* TLS_SIZE_PRE_OFFSET should be equal to sizeof(tls_tcb_t) but cpus such as mips don't respect variant I exactly */
        dynobj->tls.total_size = TLS_SIZE_PRE_OFFSET;
#elif defined(TLS_TCB_AT_TP)
        /* not really necessary, calloc already set to 0 */
        dynobj->tls.total_size = 0;
#else
#error "TLS variant not defined for your cpu"
#endif
        _tls_compute_offsets(dynobj, dynobj);

        /* TLS_SIZE_POST_OFFSET should be equal to 0 but cpus such as mips don't respect variant I exactly */
        dynobj->tls.total_size += TLS_SIZE_POST_OFFSET;

        _libelf_debug(DEBUG, "\ttls total size: 0x%x\n", dynobj->tls.total_size);
    }

    return 0;
}

error_t _tls_dynobj_size(const struct dynobj_rtld_s *dynobj, size_t *size)
{
    /* sanity check */
    assert(dynobj->tls.modid <= 1);
    if (dynobj->tls.nb_modid == 0)
    {
        _libelf_debug(DEBUG, "\tno tls for this program\n");
        return 0;
    }

    size_t dtv_size = (dynobj->tls.max_modid+1) * sizeof(tls_dtv_t);

    *size = dynobj->tls.total_size + dtv_size;

    return 0;
}

error_t _tls_init_dynobj(const struct dynobj_rtld_s *dynobj, uintptr_t tls, uintptr_t *threadpointer)
{

    uintptr_t tp;
    tls_dtv_t *dtv;
#if defined(TLS_DTVP_AT_TP)
    tp = tls + TLS_TCB_OFFSET; /* some cpu as mips, don't respect variant I exactly */
    dtv = (tls_dtv_t*)(tls + dynobj->tls.total_size); /* tls_total_size does include tcb */
#elif defined(TLS_TCB_AT_TP)
    tp = tls + dynobj->tls.total_size; /* tls_total_size does not include tcb */
    dtv = (tls_dtv_t*)(tp + sizeof(tls_tcb_t));
#else
#error "TLS variant is not defined for your cpu"
#endif

    /* fill the tls area with tlsimage for each modules */
    if (_tls_load_images(dynobj, dynobj, tp, dtv) != 0)
    {
        _libelf_debug(NONE, "\terror loading TLS images\n");
        goto err_mem;
    }

    /* init first field of DTV, "generation counter" */
    dtv[0].counter = dynobj->tls.max_modid;

    /* set the DTV pointer in TCB */
    tls_tcb_t *tcb = (tls_tcb_t*)tls;
    tcb->dtvp = dtv;

    _libelf_debug(DEBUG, "\treturn threadpointer %p\n", (void*)tp);
    /* set the threadpointer address */
    *threadpointer = tp;

    return 0;

err_mem:
    free((void*)tls);
    return -1;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


