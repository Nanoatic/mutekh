/* Copyright (C) 2005, 2006, 2008, 2009 Free Software Foundation, Inc.
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

/* This file contains system specific routines related to counting
   online processors and dynamic load balancing.  It is expected that
   a system may well want to write special versions of each of these.

   The following implementation uses a mix of POSIX and BSD routines.  */

#include "libgomp.h"
#include <unistd.h>
#include <stdlib.h>
#ifdef HAVE_GETLOADAVG
# ifdef HAVE_SYS_LOADAVG_H
#  include <sys/loadavg.h>
# endif
#endif

#include <hexo/cpu.h>

/* At startup, determine the default number of threads.  It would seem
   this should be related to the number of cpus online.  */

void
gomp_init_num_threads (void)
{
  //  gomp_global_icv.nthreads_var = arch_get_cpu_count();
  gomp_global_icv.nthreads_var = CONFIG_CPU_MAXCOUNT;
}

/* When OMP_DYNAMIC is set, at thread launch determine the number of
   threads we should spawn for this team.  */
/* ??? I have no idea what best practice for this is.  Surely some
   function of the number of processors that are *still* online and
   the load average.  Here I use the number of processors online
   minus the 15 minute load average.  */

omp_uint_t
gomp_dynamic_max_threads (void)
{
  omp_uint_t n_onln, loadavg;
  omp_uint_t nthreads_var = gomp_icv (false)->nthreads_var;

  n_onln = CONFIG_CPU_MAXCOUNT; //arch_get_cpu_count();
  if (n_onln > nthreads_var)
    n_onln = nthreads_var;

  loadavg = 0;
#ifdef HAVE_GETLOADAVG
  {
    double dloadavg[3];
    if (getloadavg (dloadavg, 3) == 3)
      {
	/* Add 0.1 to get a kind of biased rounding.  */
	loadavg = dloadavg[2] + 0.1;
      }
  }
#endif

  if (loadavg >= n_onln)
    return 1;
  else
    return n_onln - loadavg;
}

omp_int_t
omp_get_num_procs (void)
{
  return CONFIG_CPU_MAXCOUNT; //arch_get_cpu_count();
}

ialias (omp_get_num_procs)
