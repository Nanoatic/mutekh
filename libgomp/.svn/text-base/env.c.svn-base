/* Copyright (C) 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
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

/* This file defines the OpenMP internal control variables, and arranges
   for them to be initialized from environment variables at startup.  */

#include "libgomp.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

struct gomp_task_icv gomp_global_icv = {
  .nthreads_var = 1,
  .run_sched_var = GFS_DYNAMIC,
  .run_sched_modifier = 1,
  .dyn_var = false,
  .nest_var = false
};

uint16_t *gomp_cpu_affinity;
size_t gomp_cpu_affinity_len;
omp_ulong_t gomp_max_active_levels_var = INT_MAX;
omp_ulong_t gomp_thread_limit_var = ULONG_MAX;
omp_ulong_t gomp_remaining_threads_count;
#ifndef CONFIG_OPENMP_GCC_SYNC
gomp_mutex_t gomp_remaining_threads_lock;
#endif
omp_ulong_t gomp_available_cpus = 1, gomp_managed_threads = 1;
uint64_t gomp_spin_count_var, gomp_throttled_spin_count_var;

/* Parse the OMP_SCHEDULE environment variable.  */

static void
parse_schedule (void)
{
  char *env, *end;
  omp_ulong_t value;

  env = CONFIG_OPENMP_OMP_SCHEDULE;
  if (env == NULL)
    return;

  while (isspace ((uint8_t) *env))
    ++env;
  if (strncasecmp (env, "static", 6) == 0)
    {
      gomp_global_icv.run_sched_var = GFS_STATIC;
      env += 6;
    }
  else if (strncasecmp (env, "dynamic", 7) == 0)
    {
      gomp_global_icv.run_sched_var = GFS_DYNAMIC;
      env += 7;
    }
  else if (strncasecmp (env, "guided", 6) == 0)
    {
      gomp_global_icv.run_sched_var = GFS_GUIDED;
      env += 6;
    }
  else if (strncasecmp (env, "auto", 4) == 0)
    {
      gomp_global_icv.run_sched_var = GFS_AUTO;
      env += 4;
    }
  else
    goto unknown;

  while (isspace ((uint8_t) *env))
    ++env;
  if (*env == '\0')
    return;
  if (*env++ != ',')
    goto unknown;
  while (isspace ((uint8_t) *env))
    ++env;
  if (*env == '\0')
    goto invalid;

  errno = 0;
  value = strtoul (env, &end, 10);
  if (errno)
    goto invalid;

  while (isspace ((uint8_t) *end))
    ++end;
  if (*end != '\0')
    goto invalid;

  if ((omp_int_t)value != value)
    goto invalid;

  gomp_global_icv.run_sched_modifier = value;
  return;

 unknown:
  gomp_error ("Unknown value for environment variable OMP_SCHEDULE");
  return;

 invalid:
  gomp_error ("Invalid value for chunk size in "
	      "environment variable OMP_SCHEDULE");
  return;
}

/* Parse the OMP_WAIT_POLICY environment variable and store the
   result in gomp_active_wait_policy.  */

static omp_int_t
parse_wait_policy (void)
{
  const char *env;
  omp_int_t ret = -1;

  env = CONFIG_OPENMP_OMP_WAIT_POLICY;
  if (env == NULL)
    return -1;

  while (isspace ((uint8_t) *env))
    ++env;
  if (strncasecmp (env, "active", 6) == 0)
    {
      ret = 1;
      env += 6;
    }
  else if (strncasecmp (env, "passive", 7) == 0)
    {
      ret = 0;
      env += 7;
    }
  else
    env = "X";
  while (isspace ((uint8_t) *env))
    ++env;
  if (*env == '\0')
    return ret;
  gomp_error ("Invalid value for environment variable OMP_WAIT_POLICY");
  return -1;
}

/* Parse the GOMP_CPU_AFFINITY environment varible.  Return true if one was
   present and it was successfully parsed.  */

static bool
parse_affinity (void)
{
  char *env, *end;
  omp_ulong_t cpu_beg, cpu_end, cpu_stride;
  uint16_t *cpus = NULL;
  size_t allocated = 0, used = 0, needed;

  env = CONFIG_OPENMP_GOMP_CPU_AFFINITY;
  if (env == NULL)
    return false;

  do
    {
      while (*env == ' ' || *env == '\t')
	env++;

      cpu_beg = strtoul (env, &end, 0);
      cpu_end = cpu_beg;
      cpu_stride = 1;
      if (env == end || cpu_beg >= 65536)
	goto invalid;

      env = end;
      if (*env == '-')
	{
	  cpu_end = strtoul (++env, &end, 0);
	  if (env == end || cpu_end >= 65536 || cpu_end < cpu_beg)
	    goto invalid;

	  env = end;
	  if (*env == ':')
	    {
	      cpu_stride = strtoul (++env, &end, 0);
	      if (env == end || cpu_stride == 0 || cpu_stride >= 65536)
		goto invalid;

	      env = end;
	    }
	}

      needed = (cpu_end - cpu_beg) / cpu_stride + 1;
      if (used + needed >= allocated)
	{
	  uint16_t *new_cpus;

	  if (allocated < 64)
	    allocated = 64;
	  if (allocated > needed)
	    allocated <<= 1;
	  else
	    allocated += 2 * needed;
	  new_cpus = realloc (cpus, allocated * sizeof (uint16_t));
	  if (new_cpus == NULL)
	    {
	      free (cpus);
	      gomp_error ("not enough memory to store GOMP_CPU_AFFINITY list");
	      return false;
	    }

	  cpus = new_cpus;
	}

      while (needed--)
	{
	  cpus[used++] = cpu_beg;
	  cpu_beg += cpu_stride;
	}

      while (*env == ' ' || *env == '\t')
	env++;

      if (*env == ',')
	env++;
      else if (*env == '\0')
	break;
    }
  while (1);

  gomp_cpu_affinity = cpus;
  gomp_cpu_affinity_len = used;
  return true;

 invalid:
  gomp_error ("Invalid value for enviroment variable GOMP_CPU_AFFINITY");
  return false;
}

void gomp_initialize_env (void)
{
  omp_int_t wait_policy;

  parse_schedule ();

  gomp_global_icv.dyn_var = CONFIG_OPENMP_OMP_DYNAMIC;
  gomp_global_icv.nest_var = CONFIG_OPENMP_OMP_NESTED;

  gomp_max_active_levels_var = CONFIG_OPENMP_OMP_MAX_ACTIVE_LEVELS;

  gomp_thread_limit_var = CONFIG_OPENMP_OMP_THREAD_LIMIT;
  if (gomp_thread_limit_var != ULONG_MAX)
    gomp_remaining_threads_count = gomp_thread_limit_var - 1;

#ifndef CONFIG_OPENMP_GCC_SYNC
  gomp_mutex_init (&gomp_remaining_threads_lock);
#endif
  gomp_init_num_threads ();
  gomp_available_cpus = gomp_global_icv.nthreads_var;

#ifdef CONFIG_OPENMP_OMP_NUM_THREADS
  gomp_global_icv.nthreads_var = CONFIG_OPENMP_OMP_NUM_THREADS;
#else
  gomp_global_icv.nthreads_var = gomp_available_cpus;
#endif

  if (parse_affinity ())
    gomp_init_affinity ();

  wait_policy = parse_wait_policy ();

#ifdef CONFIG_OPENMP_GOMP_SPINCOUNT
  gomp_spin_count_var = CONFIG_OPENMP_GOMP_SPINCOUNT;
#else
  /* Using a rough estimation of 100000 spins per msec,
     use 5 min blocking for OMP_WAIT_POLICY=active,
     200 msec blocking when OMP_WAIT_POLICY is not specificed
     and 0 when OMP_WAIT_POLICY=passive.
     Depending on the CPU speed, this can be e.g. 5 times longer
     or 5 times shorter.  */
  if (wait_policy > 0)
    gomp_spin_count_var = 30000000000LL;
  else if (wait_policy < 0)
    gomp_spin_count_var = 20000000LL;
#endif

  /* gomp_throttled_spin_count_var is used when there are more libgomp
     managed threads than available CPUs.  Use very short spinning.  */
  if (wait_policy > 0)
    gomp_throttled_spin_count_var = 1000LL;
  else if (wait_policy < 0)
    gomp_throttled_spin_count_var = 100LL;
  if (gomp_throttled_spin_count_var > gomp_spin_count_var)
    gomp_throttled_spin_count_var = gomp_spin_count_var;

  /* Not strictly environment related, but ordering constructors is tricky.  */
  pthread_attr_init (&gomp_thread_attr);
  pthread_attr_setdetachstate (&gomp_thread_attr, PTHREAD_CREATE_DETACHED);

#ifdef CONFIG_OPENMP_OMP_STACKSIZE
    {
      omp_int_t err = pthread_attr_setstacksize (&gomp_thread_attr, CONFIG_OPENMP_OMP_STACKSIZE);

      if (err != 0)
	gomp_error ("Stack size change failed: %s", strerror (err));
    }
#endif
}


/* The public OpenMP API routines that access these variables.  */

void
omp_set_num_threads (omp_int_t n)
{
  struct gomp_task_icv *icv = gomp_icv (true);
  icv->nthreads_var = (n > 0 ? n : 1);
}

void
omp_set_dynamic (omp_int_t val)
{
  struct gomp_task_icv *icv = gomp_icv (true);
  icv->dyn_var = val;
}

omp_int_t
omp_get_dynamic (void)
{
  struct gomp_task_icv *icv = gomp_icv (false);
  return icv->dyn_var;
}

void
omp_set_nested (omp_int_t val)
{
  struct gomp_task_icv *icv = gomp_icv (true);
  icv->nest_var = val;
}

omp_int_t
omp_get_nested (void)
{
  struct gomp_task_icv *icv = gomp_icv (false);
  return icv->nest_var;
}

void
omp_set_schedule (omp_sched_t kind, omp_int_t modifier)
{
  struct gomp_task_icv *icv = gomp_icv (true);
  switch (kind)
    {
    case omp_sched_static:
      if (modifier < 1)
	modifier = 0;
      icv->run_sched_modifier = modifier;
      break;
    case omp_sched_dynamic:
    case omp_sched_guided:
      if (modifier < 1)
	modifier = 1;
      icv->run_sched_modifier = modifier;
      break;
    case omp_sched_auto:
      break;
    default:
      return;
    }
  icv->run_sched_var = kind;
}

void
omp_get_schedule (omp_sched_t *kind, omp_int_t *modifier)
{
  struct gomp_task_icv *icv = gomp_icv (false);
  *kind = icv->run_sched_var;
  *modifier = icv->run_sched_modifier;
}

omp_int_t
omp_get_max_threads (void)
{
  struct gomp_task_icv *icv = gomp_icv (false);
  return icv->nthreads_var;
}

omp_int_t
omp_get_thread_limit (void)
{
  return gomp_thread_limit_var > INT_MAX ? INT_MAX : gomp_thread_limit_var;
}

void
omp_set_max_active_levels (omp_int_t max_levels)
{
  if (max_levels > 0)
    gomp_max_active_levels_var = max_levels;
}

omp_int_t
omp_get_max_active_levels (void)
{
  return gomp_max_active_levels_var;
}

ialias (omp_set_dynamic)
ialias (omp_set_nested)
ialias (omp_set_num_threads)
ialias (omp_get_dynamic)
ialias (omp_get_nested)
ialias (omp_set_schedule)
ialias (omp_get_schedule)
ialias (omp_get_max_threads)
ialias (omp_get_thread_limit)
ialias (omp_set_max_active_levels)
ialias (omp_get_max_active_levels)
