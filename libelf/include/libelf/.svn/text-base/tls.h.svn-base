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

#ifndef _TLS_H_
#define _TLS_H_

#include <cpu/tls.h>

struct dynobj_rtld_s;

/*
 * TLS object
 */

struct dynobj_tls_s
{
    size_t  modid;      /* Index of TLS if any (0 otherwise) */
    size_t  max_modid;  /* Max TLS index in the dep chain (including the object) */
    size_t  nb_modid;   /* Number of dependencies that uses tls (including the object) */

    /* Only for program object */
    size_t  *offset_shobj;  /* offset values of the data for the program and for each dependency */
    size_t  total_size;     /* total size of tls area for that program (tcb+data+dtv) */
};

/*
 * Functions prototypes
 */

void _tls_set_modid(struct dynobj_rtld_s *dynobj);

error_t _tls_load_dynobj(struct dynobj_rtld_s *dynobj);

error_t _tls_dynobj_size(const struct dynobj_rtld_s *dynobj, size_t *size);

error_t _tls_init_dynobj(const struct dynobj_rtld_s *dynobj, uintptr_t tls, uintptr_t *threadpointer);

error_t _tls_allocate_dynobj(const struct dynobj_rtld_s *dynobj, uintptr_t *tls);

#endif /* _TLS_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

