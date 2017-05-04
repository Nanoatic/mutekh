/*
 * This file is part of DSX, development environment for static
 * SoC applications.
 * 
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) 2006, Nicolas Pouillon, <nipo@ssji.net>
 *     Laboratoire d'informatique de Paris 6 / ASIM, France
 * 
 *  $Id$
 */

#ifndef SRL_MWMR_H_
#define SRL_MWMR_H_

#include "srl_public_types.h"

/* Bloquant */
void srl_mwmr_read( srl_mwmr_t, void *, size_t );

void srl_mwmr_write( srl_mwmr_t, void *, size_t );

/* Non bloquant */
ssize_t srl_mwmr_try_read( srl_mwmr_t, void *, size_t );

ssize_t srl_mwmr_try_write( srl_mwmr_t, void *, size_t );
#endif
