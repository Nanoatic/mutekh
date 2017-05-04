/* Copyright (C) 2005, 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU OpenMP Library (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  

   MutekH port by Alexandre Becoulet, 2010

*/

/* This file contains system specific timer routines.  It is expected that
   a system may well want to write special versions of each of these.

   The following implementation uses the most simple POSIX routines.
   If present, POSIX 4 clocks should be used instead.  */

#include <hexo/cpu.h>

#include "libgomp.h"

double
omp_get_wtime (void)
{
  /* FIXME */
  return (double)cpu_cycle_count() / 1e9;
}

double
omp_get_wtick (void)
{
  /* FIXME */
  return (double)cpu_cycle_count() / 1e6;
}

ialias (omp_get_wtime)
ialias (omp_get_wtick)

