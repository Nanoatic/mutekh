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

#ifndef CAPSULE_H
#define CAPSULE_H

/**
   @file
   @module{Capsule}
   @short Capsule threading API

   Environment aiming at easing parallel programming, see
   @url https://alchemy.futurs.inria.fr/capsule/
 */

#include <stdio.h>
#include <hexo/types.h>

#include <mutek/scheduler.h>
#include <mutek/semaphore.h>
#include <hexo/context.h>
#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_slist.h>

#include <capsule_types.h>

/**
   @this is the API version of Capsule library
 */
#define CAP_API_VERSION   6

/**
   @this is the prototype for optional destructor of user data
   accessible through ctxt->private_dtor
 */
typedef void (*capsule_ctxt_user_dtor_t)(void *);

/**
   @this is the prototype for user function to call in splitted
   thread
 */
typedef void (*capsule_ctxt_func_t)(void *);

/**
   @internal

   @this is a group opaque structure, only present because referenced
   from public context struct.
 */
struct capsule_group_s;

#define CONTAINER_LOCK_capsule_queue HEXO_SPIN

CONTAINER_TYPE(capsule_queue, SLIST,
/**
   @this is a capsule context.
 */
struct capsule_ctxt_s
{
    /**
       @internal
       list entry for chaining
     */
    CONTAINER_ENTRY_TYPE(SLIST) list_entry;
    /**
       @internal
       group
     */
    struct capsule_group_s *group;

    /**
       @internal
       function to call
     */
    capsule_ctxt_func_t func;
    /**
       @internal
       argument to function
     */
    void *arg;

    /**
       private user data associated to thread
     */
    void *priv;

    /**
       private user data destructor associated to thread
     */
    capsule_ctxt_user_dtor_t private_dtor;
}, list_entry);

/**
   @this is an API typedef
 */
typedef struct capsule_ctxt_s capsule_ctxt_t;

/**
   @this initializes the Capsule system.

   @return 0 on correct completion
 */
capsule_rc_t capsule_sys_init(void);

/**
   @this prepares the Capsule system allocating internal data.

   @return 0 on correct completion
 */
capsule_rc_t capsule_sys_init_warmup(void);

/**
   @this destroys the capsule system, deallocates all data.
 */
void capsule_sys_destroy(void);

/**
   Globally disable probes, any subsequent calls to capsule_probe will
   return a NULL context.
 */
void capsule_sys_block(void);

/**
   Globally enable probes, restores usual behaviour of capsule_probe
 */
void capsule_sys_unblock(void);

/**
   Dumps various statistics about the Capsule system to the passed
   stream.

   @param stream Stdio stream to dump statistics to
 */
#ifdef CONFIG_LIBC_STREAM
void capsule_sys_dump_all_stats(FILE * stream);
#endif

/**
   Reset statistics.
 */
void capsule_sys_reset_all_stats();

/**
   @this checks a hardware thread is globally available for calling
   @tt func, if so, reserve it and return it in @tt ctxt. If thread is
   available, next call MUST be capsule_divide.

   @param func Function thread will execute
   @param ctxt Returned context if available. *ctxt == NULL if not
   available
 */
void capsule_probe(capsule_ctxt_func_t func, capsule_ctxt_t ** ctxt);

/**
   @this spawns a created thread through capsule_probe, calling the
   thread's function with @tt arg as argument.

   @param ctxt Context to run
   @param arg Argument to pass to thread function
 */
void capsule_divide(capsule_ctxt_t * ctxt, void * arg);

/**
   @this retrieves the current capsule context in order to be able to
   manipulate the user_data fields through capsule_ctxt_set_user_data,
   capsule_ctxt_get_user_data, capsule_ctxt_set_user_dtor.

   @return Current capsule context
 */
capsule_ctxt_t *capsule_ctxt_get(void);

/**
   @this sets user data associated to current context

   @param ctxt Context to attach data to
   @param user_data User data pointer to attach to context
 */
static inline
void capsule_ctxt_set_user_data(capsule_ctxt_t * ctxt, void * user_data)
{
    ctxt->priv = user_data;
}

/**
   @this retrieves user data associated to current context

   @param ctxt Context to retrieve data from
   @param user_data Pointer to where to store user data pointer
   associated to context
 */
static inline
void capsule_ctxt_get_user_data(capsule_ctxt_t const * ctxt, void ** user_data)
{
    *user_data = ctxt->priv;
}

/**
   @this sets user data destructor associated to current
   context. Destructor will be called if non-NULL when context is
   destroyed.

   @param ctxt Context to attach data to
   @param user_data User data destructor to attach to context
 */
static inline
void capsule_ctxt_set_user_dtor(capsule_ctxt_t * ctxt, capsule_ctxt_user_dtor_t user_dtor)
{
    ctxt->private_dtor = user_dtor;
}

/**
   @this creates a new group of contexts. All subsequents splits from
   this context and its children will be attached to the new group,
   until capsule_group_join is called. Calls to this function can be
   nested.
 */
void capsule_group_split(void);

/**
   @this waits all contexts from the current group are finished before
   going on.  When this call returns, the current context is attached
   back to its originating group.
 */
void capsule_group_join(void);

#endif
