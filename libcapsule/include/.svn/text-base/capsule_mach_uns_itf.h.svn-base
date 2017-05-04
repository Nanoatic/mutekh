/*
 * This file is part of MutekH.
 * 
 * MutekH is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * MutekH is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with MutekH; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009-2010
 */

#ifndef CAPSULE_ABS_UNS_ITF_H
#define CAPSULE_ABS_UNS_ITF_H

/**
   @file
   @module{Capsule}
   @short Capsule atomic API
 */

/** @this copies memory pointed by src to memory pointed by dst
 * atomically.
 */
#define capsule_mach_uns_atom_copy(src, dst)    \
    do { *(dst) = *(src); } while (0)

/** @this copies value in src to memory pointed by dst atomically.
 */
#define capsule_mach_uns_atom_copy_imm(src, dst)    \
    do { *(dst) = (src); } while (0)

#endif
