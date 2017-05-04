/* Copyright (C) 2005, 2008, 2009 Free Software Foundation, Inc.
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

/* This file contains routines for managing work-share iteration, both
   for loops and sections.  */

#include "libgomp.h"
#include <stdlib.h>


/* This function implements the STATIC scheduling method.  The caller should
   iterate *pstart <= x < *pend.  Return zero if there are more iterations
   to perform; nonzero if not.  Return less than 0 if this thread had
   received the absolutely last iteration.  */

omp_int_t
gomp_iter_static_next (omp_long_t *pstart, omp_long_t *pend)
{
  struct gomp_thread *thr = gomp_thread ();
  struct gomp_team *team = thr->ts.team;
  struct gomp_work_share *ws = thr->ts.work_share;
  omp_ulong_t nthreads = team ? team->nthreads : 1;

  if (thr->ts.static_trip == -1)
    return -1;

  /* Quick test for degenerate teams and orphaned constructs.  */
  if (nthreads == 1)
    {
      *pstart = ws->next;
      *pend = ws->end;
      thr->ts.static_trip = -1;
      return ws->next == ws->end;
    }

  /* We interpret chunk_size zero as "unspecified", which means that we
     should break up the iterations such that each thread makes only one
     trip through the outer loop.  */
  if (ws->chunk_size == 0)
    {
      omp_ulong_t n, q, i;
      omp_ulong_t s0, e0;
      omp_long_t s, e;

      if (thr->ts.static_trip > 0)
	return 1;

      /* Compute the total number of iterations.  */
      s = ws->incr + (ws->incr > 0 ? -1 : 1);
      n = (ws->end - ws->next + s) / ws->incr;
      i = thr->ts.team_id;

      /* Compute the "zero-based" start and end points.  That is, as
         if the loop began at zero and incremented by one.  */
      q = n / nthreads;
      q += (q * nthreads != n);
      s0 = q * i;
      e0 = s0 + q;
      if (e0 > n)
        e0 = n;

      /* Notice when no iterations allocated for this thread.  */
      if (s0 >= e0)
	{
	  thr->ts.static_trip = 1;
	  return 1;
	}

      /* Transform these to the actual start and end numbers.  */
      s = (omp_long_t)s0 * ws->incr + ws->next;
      e = (omp_long_t)e0 * ws->incr + ws->next;

      *pstart = s;
      *pend = e;
      thr->ts.static_trip = (e0 == n ? -1 : 1);
      return 0;
    }
  else
    {
      omp_ulong_t n, s0, e0, i, c;
      omp_long_t s, e;

      /* Otherwise, each thread gets exactly chunk_size iterations
	 (if available) each time through the loop.  */

      s = ws->incr + (ws->incr > 0 ? -1 : 1);
      n = (ws->end - ws->next + s) / ws->incr;
      i = thr->ts.team_id;
      c = ws->chunk_size;

      /* Initial guess is a C sized chunk positioned nthreads iterations
	 in, offset by our thread number.  */
      s0 = (thr->ts.static_trip * nthreads + i) * c;
      e0 = s0 + c;

      /* Detect overflow.  */
      if (s0 >= n)
	return 1;
      if (e0 > n)
	e0 = n;

      /* Transform these to the actual start and end numbers.  */
      s = (omp_long_t)s0 * ws->incr + ws->next;
      e = (omp_long_t)e0 * ws->incr + ws->next;

      *pstart = s;
      *pend = e;

      if (e0 == n)
	thr->ts.static_trip = -1;
      else
	thr->ts.static_trip++;
      return 0;
    }
}


/* This function implements the DYNAMIC scheduling method.  Arguments are
   as for gomp_iter_static_next.  This function must be called with ws->lock
   held.  */

bool
gomp_iter_dynamic_next_locked (omp_long_t *pstart, omp_long_t *pend)
{
  struct gomp_thread *thr = gomp_thread ();
  struct gomp_work_share *ws = thr->ts.work_share;
  omp_long_t start, end, chunk, left;

  start = ws->next;
  if (start == ws->end)
    return false;

  chunk = ws->chunk_size;
  left = ws->end - start;
  if (ws->incr < 0)
    {
      if (chunk < left)
	chunk = left;
    }
  else
    {
      if (chunk > left)
	chunk = left;
    }
  end = start + chunk;

  ws->next = end;
  *pstart = start;
  *pend = end;
  return true;
}


#ifdef CONFIG_OPENMP_GCC_SYNC
/* Similar, but doesn't require the lock held, and uses compare-and-swap
   instead.  Note that the only memory value that changes is ws->next.  */

bool
gomp_iter_dynamic_next (omp_long_t *pstart, omp_long_t *pend)
{
  struct gomp_thread *thr = gomp_thread ();
  struct gomp_work_share *ws = thr->ts.work_share;
  omp_long_t start, end, nend, chunk, incr;

  end = ws->end;
  incr = ws->incr;
  chunk = ws->chunk_size;

  if (__builtin_expect (ws->mode, 1))
    {
      omp_long_t tmp = __sync_fetch_and_add (&ws->next, chunk);
      if (incr > 0)
	{
	  if (tmp >= end)
	    return false;
	  nend = tmp + chunk;
	  if (nend > end)
	    nend = end;
	  *pstart = tmp;
	  *pend = nend;
	  return true;
	}
      else
	{
	  if (tmp <= end)
	    return false;
	  nend = tmp + chunk;
	  if (nend < end)
	    nend = end;
	  *pstart = tmp;
	  *pend = nend;
	  return true;
	}
    }

  start = ws->next;
  while (1)
    {
      omp_long_t left = end - start;
      omp_long_t tmp;

      if (start == end)
	return false;

      if (incr < 0)
	{
	  if (chunk < left)
	    chunk = left;
	}
      else
	{
	  if (chunk > left)
	    chunk = left;
	}
      nend = start + chunk;

      tmp = __sync_val_compare_and_swap (&ws->next, start, nend);
      if (__builtin_expect (tmp == start, 1))
	break;

      start = tmp;
    }

  *pstart = start;
  *pend = nend;
  return true;
}
#endif /* CONFIG_OPENMP_GCC_SYNC */


/* This function implements the GUIDED scheduling method.  Arguments are
   as for gomp_iter_static_next.  This function must be called with the
   work share lock held.  */

bool
gomp_iter_guided_next_locked (omp_long_t *pstart, omp_long_t *pend)
{
  struct gomp_thread *thr = gomp_thread ();
  struct gomp_work_share *ws = thr->ts.work_share;
  struct gomp_team *team = thr->ts.team;
  omp_ulong_t nthreads = team ? team->nthreads : 1;
  omp_ulong_t n, q;
  omp_long_t start, end;

  if (ws->next == ws->end)
    return false;

  start = ws->next;
  n = (ws->end - start) / ws->incr;
  q = (n + nthreads - 1) / nthreads;

  if (q < ws->chunk_size)
    q = ws->chunk_size;
  if (q <= n)
    end = start + q * ws->incr;
  else
    end = ws->end;

  ws->next = end;
  *pstart = start;
  *pend = end;
  return true;
}

#ifdef CONFIG_OPENMP_GCC_SYNC
/* Similar, but doesn't require the lock held, and uses compare-and-swap
   instead.  Note that the only memory value that changes is ws->next.  */

bool
gomp_iter_guided_next (omp_long_t *pstart, omp_long_t *pend)
{
  struct gomp_thread *thr = gomp_thread ();
  struct gomp_work_share *ws = thr->ts.work_share;
  struct gomp_team *team = thr->ts.team;
  omp_ulong_t nthreads = team ? team->nthreads : 1;
  omp_long_t start, end, nend, incr;
  omp_ulong_t chunk_size;

  start = ws->next;
  end = ws->end;
  incr = ws->incr;
  chunk_size = ws->chunk_size;

  while (1)
    {
      omp_ulong_t n, q;
      omp_long_t tmp;

      if (start == end)
	return false;

      n = (end - start) / incr;
      q = (n + nthreads - 1) / nthreads;

      if (q < chunk_size)
	q = chunk_size;
      if (__builtin_expect (q <= n, 1))
	nend = start + q * incr;
      else
	nend = end;

      tmp = __sync_val_compare_and_swap (&ws->next, start, nend);
      if (__builtin_expect (tmp == start, 1))
	break;

      start = tmp;
    }

  *pstart = start;
  *pend = nend;
  return true;
}
#endif /* CONFIG_OPENMP_GCC_SYNC */
