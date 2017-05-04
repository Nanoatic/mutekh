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
    Copyright Nicolas Pouillon <nipo@ssji.net> (c) 2009

*/

#include <device/icu.h>
#include <device/driver.h>
#include <device/device.h>
#include <hexo/ipi.h>

CPU_LOCAL struct ipi_endpoint_s ipi_endpoint = {};

error_t ipi_post(struct ipi_endpoint_s *endpoint)
{
    if (!endpoint)
        return -EOPNOTSUPP;

    return dev_icu_sendipi(endpoint->icu_dev, endpoint);
}

error_t ipi_post_rq(struct ipi_endpoint_s *endpoint, struct ipi_request_s *rq)
{
    if (ipi_queue_pushback(&endpoint->ipi_fifo, rq))
        return ipi_post(endpoint);

    return -ENOMEM;
}

void ipi_process_rq()
{
    struct ipi_request_s *rq;
    struct ipi_endpoint_s *ep = CPU_LOCAL_ADDR(ipi_endpoint);

    while ((rq = ipi_queue_pop(&ep->ipi_fifo)))
        rq->func(rq->priv);
}
