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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2009

*/

/**
 * @file
 * @module{Hexo}
 * @short Inter-processor interrupts
 */

#ifndef IPI_H_
#define IPI_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_dlist.h>
#include <gpct/cont_slist.h>
#include <hexo/gpct_lock_hexo.h>

#include "error.h"
#include "local.h"
#include <device/device.h>

#define IPI_MSG_FUNC(n) void (n)(void *priv)

typedef IPI_MSG_FUNC(ipi_msg_func_t);

#define CONTAINER_LOCK_ipi_queue HEXO_SPIN

CONTAINER_TYPE(ipi_queue, DLIST,
struct ipi_request_s
{
  ipi_msg_func_t *func;
  void *priv;
  ipi_queue_entry_t queue_entry;
}, queue_entry);

struct ipi_endpoint_s
{
    struct device_s *icu_dev;
    void *priv;
    ipi_queue_root_t ipi_fifo;
    CONTAINER_ENTRY_TYPE(SLIST) idle_cpu_queue_list_entry;
};

CONTAINER_FUNC(ipi_queue, DLIST, static inline, ipi_queue);
CONTAINER_FUNC_NOLOCK(ipi_queue, DLIST, static inline, ipi_queue_nolock);

extern CPU_LOCAL struct ipi_endpoint_s ipi_endpoint;


/**
   Send an ipi to given endpoint.

   @param endpoint Pointer to ipi endpoint
   @return zero if ipi was sent
 */
error_t ipi_post(struct ipi_endpoint_s *endpoint);

/**
   Attach the given callback for execution on target processor and
   send an ipi to given endpoint.

   @param endpoint Pointer to ipi endpoint
   @param rq Request buffer
   @return zero if message was attached and ipi sent
   @see #CPU_LOCAL_ID_ADDR
 */
error_t ipi_post_rq(struct ipi_endpoint_s *endpoint, struct ipi_request_s *rq);

/**
   Request processing of pending messages on current processor. Must
   be called from icu driver
 */
void ipi_process_rq();

/**
   Checks whether a given endpoint may receive IPIs.

   @param endpoint IPI endpoint to check
   @return whether endpoint may receive IPIs
*/
static inline
bool_t ipi_endpoint_isvalid(struct ipi_endpoint_s *endpoint)
{
    return endpoint != NULL && endpoint->icu_dev != NULL;
}

C_HEADER_END

#endif
