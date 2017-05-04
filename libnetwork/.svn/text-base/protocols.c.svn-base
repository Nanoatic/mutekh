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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/

#include <hexo/types.h>
#include <mutek/mem_alloc.h>

#include <netinet/packet.h>
#include <netinet/protos.h>

/*
 * Protocol instance constructor
 */

OBJECT_CONSTRUCTOR(net_proto_obj)
{
  const struct net_proto_desc_s		*desc = va_arg(ap, const struct net_proto_desc_s *);

  obj->desc = desc;
  obj->id = desc->id;
  obj->initialized = 0;
  if (desc->pv_size)
    {
      if ((obj->pv = mem_alloc(desc->pv_size, (mem_scope_context))) == NULL)
	return -1;
    }
  else
    obj->pv = NULL;

#ifdef CONFIG_NETWORK_PROFILING
  netobj_new[NETWORK_PROFILING_PROTO]++;
#endif

  return 0;
}

/*
 * Protocol instance destructor
 */

OBJECT_DESTRUCTOR(net_proto_obj)
{
  if (obj->desc->destroyproto != NULL && obj->initialized)
    obj->desc->destroyproto(obj);

#ifdef CONFIG_NETWORK_PROFILING
  netobj_del[NETWORK_PROFILING_PROTO]++;
#endif
}
