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

#ifndef CAPSULE_MACH_64BITS_ITF_H
#define CAPSULE_MACH_64BITS_ITF_H

/**
   @file
   @module{Capsule}
   @short Capsule 64-bit computing API
 */

#include <hexo/types.h>
#include <capsule_types.h>

typedef uint64_t capsule_mach_64bits_t;

/**
   @this sets 32 LSBs of a 64-bit word. MSBs are not modified

   @param src Pointer to source 32-bit word
   @param dst Pointer to destination 64-bit word
 */
static inline
void capsule_mach_64bits_set32l(
    const uint32_t * src,
    capsule_mach_64bits_t * dst)
{
    *dst &= ~(uint32_t)(-1);
    *dst |= *src;
}

/**
   @this sets 32 LSBs of a 64-bit word. MSBs are not modified

   @param src Source 32-bit word
   @param dst Pointer to destination 64-bit word
 */
static inline
void capsule_mach_64bits_set32l_imm(
    const uint32_t src,
    capsule_mach_64bits_t * dst)
{
    *dst &= ~(uint32_t)(-1);
    *dst |= src;
}

/**
   @this sets 32 LSBs of a 64-bit word. MSBs are zeroed

   @param src Pointer to source 32-bit word
   @param dst Pointer to destination 64-bit word
 */
static inline
void capsule_mach_64bits_set32z(
    const uint32_t * src,
    capsule_mach_64bits_t * dst)
{
    *dst = *src;
}

/**
   @this sets 32 LSBs of a 64-bit word. MSBs are zeroed

   @param src Source 32-bit word
   @param dst Pointer to destination 64-bit word
 */
static inline
void capsule_mach_64bits_set32z_imm(
    const uint32_t src,
    capsule_mach_64bits_t * dst)
{
    *dst = src;
}

/**
   @this sets 32 MSBs of a 64-bit word. LSBs are not modified

   @param src Pointer to source 32-bit word
   @param dst Pointer to destination 64-bit word
 */
static inline
void capsule_mach_64bits_set32h(
    const uint32_t * src,
    capsule_mach_64bits_t * dst)
{
    *dst &= (uint32_t)(-1);
    *dst |= (uint64_t)(*src) << 32;
}

/**
   @this sets 32 MSBs of a 64-bit word. LSBs are not modified

   @param src Source 32-bit word
   @param dst Pointer to destination 64-bit word
 */
static inline
void capsule_mach_64bits_set32h_imm(
    const uint32_t src,
    capsule_mach_64bits_t * dst)
{
    *dst &= (uint32_t)(-1);
    *dst |= (uint64_t)(src) << 32;
}

/**
   @this retrieves 32 MSBs of a 64-bit word.

   @param a Pointer to source 64-bit word
   @param dest Pointer to destination 32-bit word
 */
static inline
void capsule_mach_64bits_get32h(
	capsule_mach_64bits_t *a,
	uint32_t *dest)
{
	*dest = (*a)>>32;
}

/**
   @this retrieves 32 LSBs of a 64-bit word.

   @param a Pointer to source 64-bit word
   @param dest Pointer to destination 32-bit word
 */
static inline
void capsule_mach_64bits_get32l(
	capsule_mach_64bits_t *a,
	uint32_t *dest)
{
	*dest = *a;
}

/**
   @this sums two 64-bit words

   @param a Pointer to source 64-bit word
   @param b Pointer to second source 64-bit word
   @param sum Pointer to destination 64-bit word
   @param rc Pointer to a condition code
 */
static inline
void capsule_mach_64bits_add(
    capsule_rc_t * rc,
    const capsule_mach_64bits_t *a,
    const capsule_mach_64bits_t *b,
    capsule_mach_64bits_t * sum)
{
	*sum = *a+*b;
	*rc = 0;
}

/**
   @this sums a 64-bit word with a 32-bit immediate

   @param a Source 32-bit immediate word
   @param b Pointer to source 64-bit word
   @param sum Pointer to destination 64-bit word
   @param rc Pointer to a condition code
 */
static inline
void capsule_mach_64bits_add32_imm(
    capsule_rc_t * rc,
    uint32_t a,
    capsule_mach_64bits_t const *b,
    capsule_mach_64bits_t *sum)
{
	*sum = *b+a;
	*rc = 0;
}

/**
   @this computes difference of two 64-bit words

   @param a Pointer to source 64-bit word
   @param b Pointer to second source 64-bit word
   @param dif Pointer to destination 64-bit word, *dif = *a-*b
   @param rc Pointer to a condition code
 */
static inline
void capsule_mach_64bits_sub(
	capsule_rc_t * rc,
	const capsule_mach_64bits_t *a,
	const capsule_mach_64bits_t *b,
	capsule_mach_64bits_t *dif)
{
	*dif = *a-*b;
	*rc = 0;
}

/**
   @this converts a 64-bit word to a float

   @param val Pointer to source 64-bit word
   @param fl_val Pointer to destination float
 */
static inline
void capsule_mach_64bits_to_float(
    capsule_mach_64bits_t const * val,
    float * fl_val)
{
    *fl_val = (float)(*val);
}



#endif
