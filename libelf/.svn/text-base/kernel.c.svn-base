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

    Based on OSKit
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtld/rtld.h>

struct kernel_dynobj_s
{
    dynobj_list_root_t dynobj_root; /* List of kernel dynamic objects (included itself) */
} kernel_dynobj_list;

error_t rtld_kernel_init (const unsigned char *pathname)
{
	_rtld_debug(DEBUG, "rtld_kernel_init\n");

	dynobj_list_init(&kernel_dynobj_list.dynobj_root);

    return 0;
}

error_t rtld_kernel_dlopen (const unsigned char *pathname, void **handle)
{
    //TODO
	_rtld_debug(DEBUG, "rtld_kernel_dlopen\n");

    return 0;
}

error_t rtld_kernel_dlsym (const void *handle, const unsigned char *name, void **sym)
{
    //TODO
	_rtld_debug(DEBUG, "rtld_kernel_dlsym\n");

	return 0;
}

error_t rtld_kernel_dlclose (const void *handle)
{
    //TODO
	_rtld_debug(DEBUG, "rtld_kernel_dlclose\n");

    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

