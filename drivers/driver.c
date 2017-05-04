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

    Copyright (c) 2009, Nicolas Pouillon, <nipo@ssji.net>
*/

#include <device/device.h>
#include <device/driver.h>
#include <hexo/error.h>
#include <mutek/mem_alloc.h>


/**
   A global driver registry, contains pointers to all compiled-in
   drivers registered with the REGISTER_DRIVER() macro.

   This table ends at global_driver_registry_end

   Beware the table may contain NULL entries on heterogeneous
   builds. These entries are tipically a driver available only for one
   CPU (CPU ICU drivers for instance)
 */
extern const struct driver_s * global_driver_registry[];
extern const struct driver_s * global_driver_registry_end[];

const struct driver_s *driver_get_matching_pci(
	uint16_t vendor,
	uint16_t device,
	uint32_t class)
{
	const struct driver_s **drv = global_driver_registry;

	for ( ; drv < global_driver_registry_end ; drv++ ) {
		if ( !*drv )
			continue;

		const struct devenum_ident_s *ident = (*drv)->id_table;

		if ( !ident )
			continue;

		for ( ; ident->type != 0; ident++ ) {
			if ( (ident->type == DEVENUM_TYPE_PCI)
				 && (ident->pci.vendor == vendor || ident->pci.vendor == (uint16_t)-1)
				 && (ident->pci.device == device || ident->pci.device == (uint16_t)-1)
				 && (ident->pci.class == class || ident->pci.class == (uint32_t)-1) )
				return *drv;
		}
	}
	return NULL;
}

const struct driver_s *driver_get_matching_isa(
	uint16_t vendor)
{
	const struct driver_s **drv = global_driver_registry;

	for ( ; drv < global_driver_registry_end ; drv++ ) {
		if ( !*drv )
			continue;

		const struct devenum_ident_s *ident = (*drv)->id_table;

		if ( !ident )
			continue;

		for ( ; ident->type != 0; ident++ ) {
			if ( (ident->type == DEVENUM_TYPE_ISA)
				&& (ident->isa.vendor == vendor) )
				return *drv;
		}
	}
	return NULL;
}


const struct driver_s *driver_get_matching_ata(
	const char *name)
{
	const struct driver_s **drv = global_driver_registry;

	for ( ; drv < global_driver_registry_end ; drv++ ) {
		if ( !*drv )
			continue;

		const struct devenum_ident_s *ident = (*drv)->id_table;

		if ( !ident )
			continue;

		for ( ; ident->type != 0; ident++ ) {
			if ( (ident->type == DEVENUM_TYPE_ATA)
				 && !strcmp(ident->ata.str, name) )
				return *drv;
		}
	}
	return NULL;
}


const struct driver_s *driver_get_matching_fdtname(
	const char *name)
{
	const struct driver_s **drv = global_driver_registry;

	for ( ; drv < global_driver_registry_end ; drv++ ) {
		if ( !*drv )
			continue;

		const struct devenum_ident_s *ident = (*drv)->id_table;

		if ( !ident )
			continue;

		for ( ; ident->type != 0; ident++ ) {
			if ( (ident->type == DEVENUM_TYPE_FDTNAME)
				 && !strcmp(ident->fdtname.name, name) ) {
				return *drv;
			}
		}
	}
	return NULL;
}

