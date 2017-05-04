/* Copyright (C) 2005, 2007, 2008, 2009 Free Software Foundation, Inc.
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

/* This file contains data types and function declarations that are not
   part of the official OpenMP user interface.  There are declarations
   in here that are part of the GNU OpenMP ABI, in that the compiler is
   required to know about them and use them.

   The convention is that the all caps prefix "GOMP" is used group items
   that are part of the external ABI, and the lower case prefix "gomp"
   is used group items that are completely private to the library.  */

#ifndef LIBGOMP_H 
#define LIBGOMP_H 1

#include <stdbool.h>
#include <pthread.h>
#include <mutek/printk.h>

#include <omp.h>

#include "posix/sem.h"
#include "posix/mutex.h"
#include "posix/bar.h"
#include "posix/ptrlock.h"


/* This structure contains the data to control one work-sharing construct,
   either a LOOP (FOR/DO) or a SECTIONS.  */

enum gomp_schedule_type
{
  GFS_RUNTIME,
  GFS_STATIC,
  GFS_DYNAMIC,
  GFS_GUIDED,
  GFS_AUTO
};

struct gomp_work_share
{
  /* This member records the SCHEDULE clause to be used for this construct.
     The user specification of "runtime" will already have been resolved.
     If this is a SECTIONS construct, this value will always be DYNAMIC.  */
  enum gomp_schedule_type sched;

  omp_int_t mode;

  union {
    struct {
      /* This is the chunk_size argument to the SCHEDULE clause.  */
      omp_long_t chunk_size;

      /* This is the iteration end point.  If this is a SECTIONS construct,
	 this is the number of contained sections.  */
      omp_long_t end;

      /* This is the iteration step.  If this is a SECTIONS construct, this
	 is always 1.  */
      omp_long_t incr;
    };

    struct {
      /* The same as above, but for the uint64_t loop variants.  */
      uint64_t chunk_size_ull;
      uint64_t end_ull;
      uint64_t incr_ull;
    };
  };

  /* This is a circular queue that details which threads will be allowed
     into the ordered region and in which order.  When a thread allocates
     iterations on which it is going to work, it also registers itself at
     the end of the array.  When a thread reaches the ordered region, it
     checks to see if it is the one at the head of the queue.  If not, it
     blocks on its RELEASE semaphore.  */
  omp_uint_t *ordered_team_ids;

  /* This is the number of threads that have registered themselves in
     the circular queue ordered_team_ids.  */
  omp_uint_t ordered_num_used;

  /* This is the team_id of the currently acknowledged owner of the ordered
     section, or -1u if the ordered section has not been acknowledged by
     any thread.  This is distinguished from the thread that is *allowed*
     to take the section next.  */
  omp_uint_t ordered_owner;

  /* This is the index into the circular queue ordered_team_ids of the
     current thread that's allowed into the ordered reason.  */
  omp_uint_t ordered_cur;

  /* This is a chain of allocated gomp_work_share blocks, valid only
     in the first gomp_work_share struct in the block.  */
  struct gomp_work_share *next_alloc;

  /* The above fields are written once during workshare initialization,
     or related to ordered worksharing.  Make sure the following fields
     are in a different cache line.  */

  /* This lock protects the update of the following members.  */
  gomp_mutex_t lock __attribute__((aligned (64)));

  /* This is the count of the number of threads that have exited the work
     share construct.  If the construct was marked nowait, they have moved on
     to other work; otherwise they're blocked on a barrier.  The last member
     of the team to exit the work share construct must deallocate it.  */
  omp_uint_t threads_completed;

  union {
    /* This is the next iteration value to be allocated.  In the case of
       GFS_STATIC loops, this the iteration start point and never changes.  */
    omp_long_t next;

    /* The same, but with uint64_t type.  */
    uint64_t next_ull;

    /* This is the returned data structure for SINGLE COPYPRIVATE.  */
    void *copyprivate;
  };

  union {
    /* Link to gomp_work_share struct for next work sharing construct
       encountered after this one.  */
    gomp_ptrlock_t next_ws;

    /* gomp_work_share structs are chained in the free work share cache
       through this.  */
    struct gomp_work_share *next_free;
  };

  /* If only few threads are in the team, ordered_team_ids can point
     to this array which fills the padding at the end of this struct.  */
  omp_uint_t inline_ordered_team_ids[0];
};

/* This structure contains all of the thread-local data associated with 
   a thread team.  This is the data that must be saved when a thread
   encounters a nested PARALLEL construct.  */

struct gomp_team_state
{
  /* This is the team of which the thread is currently a member.  */
  struct gomp_team *team;

  /* This is the work share construct which this thread is currently
     processing.  Recall that with NOWAIT, not all threads may be 
     processing the same construct.  */
  struct gomp_work_share *work_share;

  /* This is the previous work share construct or NULL if there wasn't any.
     When all threads are done with the current work sharing construct,
     the previous one can be freed.  The current one can't, as its
     next_ws field is used.  */
  struct gomp_work_share *last_work_share;

  /* This is the ID of this thread within the team.  This value is
     guaranteed to be between 0 and N-1, where N is the number of
     threads in the team.  */
  omp_uint_t team_id;

  /* Nesting level.  */
  omp_uint_t level;

  /* Active nesting level.  Only active parallel regions are counted.  */
  omp_uint_t active_level;

#ifdef CONFIG_OPENMP_GCC_SYNC
  /* Number of single stmts encountered.  */
  omp_ulong_t single_count;
#endif

  /* For GFS_RUNTIME loops that resolved to GFS_STATIC, this is the
     trip number through the loop.  So first time a particular loop
     is encountered this number is 0, the second time through the loop
     is 1, etc.  This is unused when the compiler knows in advance that
     the loop is statically scheduled.  */
  omp_ulong_t static_trip;
};

/* These are the OpenMP 3.0 Internal Control Variables described in
   section 2.3.1.  Those described as having one copy per task are
   stored within the structure; those described as having one copy
   for the whole program are (naturally) global variables.  */

struct gomp_task_icv
{
  omp_ulong_t nthreads_var;
  enum gomp_schedule_type run_sched_var;
  omp_int_t run_sched_modifier;
  bool dyn_var;
  bool nest_var;
};

extern struct gomp_task_icv gomp_global_icv;
extern omp_ulong_t gomp_thread_limit_var;
extern omp_ulong_t gomp_remaining_threads_count;
#ifndef CONFIG_OPENMP_GCC_SYNC
extern gomp_mutex_t gomp_remaining_threads_lock;
#endif
extern omp_ulong_t gomp_max_active_levels_var;
extern uint64_t gomp_spin_count_var, gomp_throttled_spin_count_var;
extern omp_ulong_t gomp_available_cpus, gomp_managed_threads;

enum gomp_task_kind
{
  GOMP_TASK_IMPLICIT,
  GOMP_TASK_IFFALSE,
  GOMP_TASK_WAITING,
  GOMP_TASK_TIED
};

/* This structure describes a "task" to be run by a thread.  */

struct gomp_task
{
  struct gomp_task *parent;
  struct gomp_task *children;
  struct gomp_task *next_child;
  struct gomp_task *prev_child;
  struct gomp_task *next_queue;
  struct gomp_task *prev_queue;
  struct gomp_task_icv icv;
  void (*fn) (void *);
  void *fn_data;
  enum gomp_task_kind kind;
  bool in_taskwait;
  bool in_tied_task;
  gomp_sem_t taskwait_sem;
};

/* This structure describes a "team" of threads.  These are the threads
   that are spawned by a PARALLEL constructs, as well as the work sharing
   constructs that the team encounters.  */

struct gomp_team
{
  /* This is the number of threads in the current team.  */
  omp_uint_t nthreads;

  /* This is number of gomp_work_share structs that have been allocated
     as a block last time.  */
  omp_uint_t work_share_chunk;

  /* This is the saved team state that applied to a master thread before
     the current thread was created.  */
  struct gomp_team_state prev_ts;

  /* This semaphore should be used by the master thread instead of its
     "native" semaphore in the thread structure.  Required for nested
     parallels, as the master is a member of two teams.  */
  gomp_sem_t master_release;

  /* This points to an array with pointers to the release semaphore
     of the threads in the team.  */
  gomp_sem_t **ordered_release;

  /* List of gomp_work_share structs chained through next_free fields.
     This is populated and taken off only by the first thread in the
     team encountering a new work sharing construct, in a critical
     section.  */
  struct gomp_work_share *work_share_list_alloc;

  /* List of gomp_work_share structs freed by free_work_share.  New
     entries are atomically added to the start of the list, and
     alloc_work_share can safely only move all but the first entry
     to work_share_list alloc, as free_work_share can happen concurrently
     with alloc_work_share.  */
  struct gomp_work_share *work_share_list_free;

#ifdef CONFIG_OPENMP_GCC_SYNC
  /* Number of simple single regions encountered by threads in this
     team.  */
  omp_ulong_t single_count;
#else
  /* Mutex protecting addition of workshares to work_share_list_free.  */
  gomp_mutex_t work_share_list_free_lock;
#endif

  /* This barrier is used for most synchronization of the team.  */
  gomp_barrier_t barrier;

  /* Initial work shares, to avoid allocating any gomp_work_share
     structs in the common case.  */
  struct gomp_work_share work_shares[8];

  gomp_mutex_t task_lock;
  struct gomp_task *task_queue;
  omp_int_t task_count;
  omp_int_t task_running_count;

  /* This array contains structures for implicit tasks.  */
  struct gomp_task implicit_task[];
};

/* This structure contains all data that is private to libgomp and is
   allocated per thread.  */

struct gomp_thread
{
  /* This is the function that the thread should run upon launch.  */
  void (*fn) (void *data);
  void *data;

  /* This is the current team state for this thread.  The ts.team member
     is NULL only if the thread is idle.  */
  struct gomp_team_state ts;

  /* This is the task that the thread is currently executing.  */
  struct gomp_task *task;

  /* This semaphore is used for ordered loops.  */
  gomp_sem_t release;

  /* user pthread thread pool */
  struct gomp_thread_pool *thread_pool;
};


struct gomp_thread_pool
{
  /* This array manages threads spawned from the top level, which will
     return to the idle loop once the current PARALLEL construct ends.  */
  struct gomp_thread **threads;
  omp_uint_t threads_size;
  omp_uint_t threads_used;
  struct gomp_team *last_team;

  /* This barrier holds and releases threads waiting in threads.  */
  gomp_barrier_t threads_dock;
};

/* ... and here is that TLS data.  */

extern CONTEXT_LOCAL struct gomp_thread gomp_tls_data;

static inline struct gomp_thread *gomp_thread (void)
{
  return CONTEXT_LOCAL_ADDR(gomp_tls_data);
}

extern struct gomp_task_icv *gomp_new_icv (void);

/* Here's how to access the current copy of the ICVs.  */

static inline struct gomp_task_icv *gomp_icv (bool write)
{
  struct gomp_task *task = gomp_thread ()->task;
  if (task)
    return &task->icv;
  else if (write)
    return gomp_new_icv ();
  else
    return &gomp_global_icv;
}

/* The attributes to be used during thread creation.  */
extern pthread_attr_t gomp_thread_attr;

/* Other variables.  */

extern uint16_t *gomp_cpu_affinity;
extern size_t gomp_cpu_affinity_len;

/* Function prototypes.  */

/* affinity.c */

extern void gomp_init_affinity (void);
extern void gomp_init_thread_affinity (pthread_attr_t *);

/* alloc.c */

extern void *gomp_malloc (size_t) __attribute__((malloc));
extern void *gomp_malloc_cleared (size_t) __attribute__((malloc));
extern void *gomp_realloc (void *, size_t);

/* Avoid conflicting prototypes of alloca() in system headers by using
   GCC's builtin alloca().  */
#define gomp_alloca(x)  __builtin_alloca(x)

/* error.c */

#define gomp_error(...) printk("libgomp: "__VA_ARGS__)
#define gomp_fatal(...) do { printk("libgomp: "__VA_ARGS__); abort(); } while (0)

/* iter.c */

extern omp_int_t gomp_iter_static_next (omp_long_t *, omp_long_t *);
extern bool gomp_iter_dynamic_next_locked (omp_long_t *, omp_long_t *);
extern bool gomp_iter_guided_next_locked (omp_long_t *, omp_long_t *);

#ifdef CONFIG_OPENMP_GCC_SYNC
extern bool gomp_iter_dynamic_next (omp_long_t *, omp_long_t *);
extern bool gomp_iter_guided_next (omp_long_t *, omp_long_t *);
#endif

/* iter_ull.c */

extern omp_int_t gomp_iter_ull_static_next (uint64_t *,
				      uint64_t *);
extern bool gomp_iter_ull_dynamic_next_locked (uint64_t *,
					       uint64_t *);
extern bool gomp_iter_ull_guided_next_locked (uint64_t *,
					      uint64_t *);

#if defined CONFIG_OPENMP_GCC_SYNC && defined __LP64__
extern bool gomp_iter_ull_dynamic_next (uint64_t *,
					uint64_t *);
extern bool gomp_iter_ull_guided_next (uint64_t *,
				       uint64_t *);
#endif

/* ordered.c */

extern void gomp_ordered_first (void);
extern void gomp_ordered_last (void);
extern void gomp_ordered_next (void);
extern void gomp_ordered_static_init (void);
extern void gomp_ordered_static_next (void);
extern void gomp_ordered_sync (void);

/* parallel.c */

extern omp_uint_t gomp_resolve_num_threads (omp_uint_t, omp_uint_t);

/* proc.c (in config/) */

extern void gomp_init_num_threads (void);
extern omp_uint_t gomp_dynamic_max_threads (void);

/* task.c */

extern void gomp_init_task (struct gomp_task *, struct gomp_task *,
			    struct gomp_task_icv *);
extern void gomp_end_task (void);
extern void gomp_barrier_handle_tasks (gomp_barrier_state_t);

static void inline
gomp_finish_task (struct gomp_task *task)
{
  gomp_sem_destroy (&task->taskwait_sem);
}

/* team.c */

extern struct gomp_team *gomp_new_team (omp_uint_t);
extern void gomp_team_start (void (*) (void *), void *, omp_uint_t,
			     struct gomp_team *);
extern void gomp_team_end (void);

/* work.c */

extern void gomp_init_work_share (struct gomp_work_share *, bool, omp_uint_t);
extern void gomp_fini_work_share (struct gomp_work_share *);
extern bool gomp_work_share_start (bool);
extern void gomp_work_share_end (void);
extern void gomp_work_share_end_nowait (void);

static inline void
gomp_work_share_init_done (void)
{
  struct gomp_thread *thr = gomp_thread ();
  if (__builtin_expect (thr->ts.last_work_share != NULL, 1))
    gomp_ptrlock_set (&thr->ts.last_work_share->next_ws, thr->ts.work_share);
}

/* barrier.c */

extern void GOMP_barrier (void);

/* critical.c */

extern void GOMP_critical_start (void);
extern void GOMP_critical_end (void);
extern void GOMP_critical_name_start (void **);
extern void GOMP_critical_name_end (void **);
extern void GOMP_atomic_start (void);
extern void GOMP_atomic_end (void);

/* loop.c */

extern bool GOMP_loop_static_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t, omp_long_t *, omp_long_t *);
extern bool GOMP_loop_dynamic_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t, omp_long_t *, omp_long_t *);
extern bool GOMP_loop_guided_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t, omp_long_t *, omp_long_t *);
extern bool GOMP_loop_runtime_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t *, omp_long_t *);

extern bool GOMP_loop_ordered_static_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t,
					    omp_long_t *, omp_long_t *);
extern bool GOMP_loop_ordered_dynamic_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t,
					     omp_long_t *, omp_long_t *);
extern bool GOMP_loop_ordered_guided_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t,
					    omp_long_t *, omp_long_t *);
extern bool GOMP_loop_ordered_runtime_start (omp_long_t, omp_long_t, omp_long_t, omp_long_t *, omp_long_t *);

extern bool GOMP_loop_static_next (omp_long_t *, omp_long_t *);
extern bool GOMP_loop_dynamic_next (omp_long_t *, omp_long_t *);
extern bool GOMP_loop_guided_next (omp_long_t *, omp_long_t *);
extern bool GOMP_loop_runtime_next (omp_long_t *, omp_long_t *);

extern bool GOMP_loop_ordered_static_next (omp_long_t *, omp_long_t *);
extern bool GOMP_loop_ordered_dynamic_next (omp_long_t *, omp_long_t *);
extern bool GOMP_loop_ordered_guided_next (omp_long_t *, omp_long_t *);
extern bool GOMP_loop_ordered_runtime_next (omp_long_t *, omp_long_t *);

extern void GOMP_parallel_loop_static_start (void (*)(void *), void *,
					     omp_uint_t, omp_long_t, omp_long_t, omp_long_t, omp_long_t);
extern void GOMP_parallel_loop_dynamic_start (void (*)(void *), void *,
					     omp_uint_t, omp_long_t, omp_long_t, omp_long_t, omp_long_t);
extern void GOMP_parallel_loop_guided_start (void (*)(void *), void *,
					     omp_uint_t, omp_long_t, omp_long_t, omp_long_t, omp_long_t);
extern void GOMP_parallel_loop_runtime_start (void (*)(void *), void *,
					      omp_uint_t, omp_long_t, omp_long_t, omp_long_t);

extern void GOMP_loop_end (void);
extern void GOMP_loop_end_nowait (void);

/* loop_ull.c */

extern bool GOMP_loop_ull_static_start (bool, uint64_t,
					uint64_t,
					uint64_t,
					uint64_t,
					uint64_t *,
					uint64_t *);
extern bool GOMP_loop_ull_dynamic_start (bool, uint64_t,
					 uint64_t,
					 uint64_t,
					 uint64_t,
					 uint64_t *,
					 uint64_t *);
extern bool GOMP_loop_ull_guided_start (bool, uint64_t,
					uint64_t,
					uint64_t,
					uint64_t,
					uint64_t *,
					uint64_t *);
extern bool GOMP_loop_ull_runtime_start (bool, uint64_t,
					 uint64_t,
					 uint64_t,
					 uint64_t *,
					 uint64_t *);

extern bool GOMP_loop_ull_ordered_static_start (bool, uint64_t,
						uint64_t,
						uint64_t,
						uint64_t,
						uint64_t *,
						uint64_t *);
extern bool GOMP_loop_ull_ordered_dynamic_start (bool, uint64_t,
						 uint64_t,
						 uint64_t,
						 uint64_t,
						 uint64_t *,
						 uint64_t *);
extern bool GOMP_loop_ull_ordered_guided_start (bool, uint64_t,
						uint64_t,
						uint64_t,
						uint64_t,
						uint64_t *,
						uint64_t *);
extern bool GOMP_loop_ull_ordered_runtime_start (bool, uint64_t,
						 uint64_t,
						 uint64_t,
						 uint64_t *,
						 uint64_t *);

extern bool GOMP_loop_ull_static_next (uint64_t *,
				       uint64_t *);
extern bool GOMP_loop_ull_dynamic_next (uint64_t *,
					uint64_t *);
extern bool GOMP_loop_ull_guided_next (uint64_t *,
				       uint64_t *);
extern bool GOMP_loop_ull_runtime_next (uint64_t *,
					uint64_t *);

extern bool GOMP_loop_ull_ordered_static_next (uint64_t *,
					       uint64_t *);
extern bool GOMP_loop_ull_ordered_dynamic_next (uint64_t *,
						uint64_t *);
extern bool GOMP_loop_ull_ordered_guided_next (uint64_t *,
					       uint64_t *);
extern bool GOMP_loop_ull_ordered_runtime_next (uint64_t *,
						uint64_t *);

/* ordered.c */

extern void GOMP_ordered_start (void);
extern void GOMP_ordered_end (void);

/* parallel.c */

extern void GOMP_parallel_start (void (*) (void *), void *, omp_uint_t);
extern void GOMP_parallel_end (void);

/* team.c */

extern void GOMP_task (void (*) (void *), void *, void (*) (void *, void *),
		       omp_long_t, omp_long_t, bool, omp_uint_t);
extern void GOMP_taskwait (void);

/* sections.c */

extern omp_uint_t GOMP_sections_start (omp_uint_t);
extern omp_uint_t GOMP_sections_next (void);
extern void GOMP_parallel_sections_start (void (*) (void *), void *,
					  omp_uint_t, omp_uint_t);
extern void GOMP_sections_end (void);
extern void GOMP_sections_end_nowait (void);

/* single.c */

extern bool GOMP_single_start (void);
extern void *GOMP_single_copy_start (void);
extern void GOMP_single_copy_end (void *);

# define gomp_init_lock_30 omp_init_lock
# define gomp_destroy_lock_30 omp_destroy_lock
# define gomp_set_lock_30 omp_set_lock
# define gomp_unset_lock_30 omp_unset_lock
# define gomp_test_lock_30 omp_test_lock
# define gomp_init_nest_lock_30 omp_init_nest_lock
# define gomp_destroy_nest_lock_30 omp_destroy_nest_lock
# define gomp_set_nest_lock_30 omp_set_nest_lock
# define gomp_unset_nest_lock_30 omp_unset_nest_lock
# define gomp_test_nest_lock_30 omp_test_nest_lock

# define ialias(fn) \
  extern __typeof (fn) gomp_ialias_##fn \
    __attribute__ ((alias (#fn)));

#endif /* LIBGOMP_H */
