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

/*
 * NFS version 2 lightweight client (with multithreading)
 *
 * Supported NFS operations:
 *
 *  + MOUNT: mount an export
 *  + UMOUNT: unmount an export
 *  + UMOUNTALL: unmount all exports
 *  + GETATTR: get attributes of a file
 *  + SETATTR: set attributes of a file
 *  + LOOKUP: get handle to a file
 *  + READ: read data from a file
 *  + STATFS: general information on filesystem
 *  + CREATE: create a file
 *  + REMOVE: remove a file
 *  + MKDIR: make a directory
 *  + RMDIR: remove a directory
 *  + WRITE: write data to a file
 *  + READDIR: read directory content
 *
 * Coming soon:
 *
 *  + READLINK: follow a symbolic link
 *  + RENAME: rename a file
 *  + LINK: create a hard link
 *  + SYMLINK: create a symbolic link
 */

#include <hexo/types.h>
#include <netinet/in.h>
#include <netinet/udp.h>

#include <netinet/nfs.h>
#include <mutek/printk.h>

#include <semaphore.h>
#include <mutek/timer.h>

CONTAINER_FUNC(rpcb, HASHLIST, static inline, rpcb, id);
CONTAINER_KEY_FUNC(rpcb, HASHLIST, static inline, rpcb, id);

/*
 * RPC timeout.
 */

TIMER_CALLBACK(rpc_timeout)
{
  struct rpcb_s	*rpcb = (struct rpcb_s *)pv;

  if (rpcb->data == NULL)
    {
      /* wake up */
      semaphore_give(&rpcb->sem, 1);
    }
}

/*
 * Receive RPC replies.
 */

UDP_CALLBACK(rpc_callback)
{
  struct nfs_s	*server = (struct nfs_s *)pv;
  struct rpcb_s	*rpcb;
  uint_fast32_t	id;

  id = ((struct rpc_reply_s *)data)->id;

  if ((rpcb = rpcb_lookup(&server->rpc_blocks, id)) != NULL)
    {
      /* duplicate the buffer */
      if ((rpcb->data = mem_alloc(size, (mem_scope_sys))) == NULL)
	return;
      memcpy(rpcb->data, data, size);
      rpcb->size = size;

      /* cancel the timeout */
      timer_cancel_event(&rpcb->timeout, 0);

      /* wake up */
      semaphore_give(&rpcb->sem, 1);
    }
}

/*
 * Endianness translation
 */

static inline void	attr_endian(struct nfs_attr_s	*attr)
{
  attr->ftype = ntohl(attr->ftype);
  attr->mode = ntohl(attr->mode);
  attr->nlink = ntohl(attr->nlink);
  attr->uid = ntohl(attr->uid);
  attr->gid = ntohl(attr->gid);
  attr->size = ntohl(attr->size);
  attr->blocksize = ntohl(attr->blocksize);
  attr->rdev = ntohl(attr->rdev);
  attr->blocks = ntohl(attr->blocks);
  attr->fsid = ntohl(attr->fsid);
  attr->fileid = ntohl(attr->fileid);
  attr->atime.tv_sec = ntohl(attr->atime.tv_sec);
  attr->atime.tv_usec = ntohl(attr->atime.tv_usec);
  attr->ctime.tv_sec = ntohl(attr->ctime.tv_sec);
  attr->ctime.tv_usec = ntohl(attr->ctime.tv_usec);
  attr->mtime.tv_sec = ntohl(attr->mtime.tv_sec);
  attr->mtime.tv_usec = ntohl(attr->mtime.tv_usec);
}

static inline void	stat_endian(struct nfs_statfs_s	*stat)
{
  stat->transfer_unit = ntohl(stat->transfer_unit);
  stat->block_size = ntohl(stat->block_size);
  stat->blocks = ntohl(stat->blocks);
  stat->blocks_free = ntohl(stat->blocks_free);
  stat->blocks_avail = ntohl(stat->blocks_avail);
}

/*
 * Do RPC call.
 */

static error_t		do_rpc(struct nfs_s	*server,
			       uint_fast32_t	program,
			       uint_fast32_t	version,
			       uint_fast32_t	procedure,
			       void		**data,
			       size_t		*size)
{
  struct net_udp_addr_s	*dest;
  struct rpc_call_s	*call;
  struct rpc_reply_s	*reply;
  struct rpcb_s		rpcb;
  void			*pkt;
  void			*p;
  size_t		sz = *size;
  uint_fast8_t		tries = 0;

  /* build the call packet */
  if ((pkt = mem_alloc(sizeof (struct rpc_call_s) + sz, (mem_scope_context))) == NULL)
    return -ENOMEM;
  call = (struct rpc_call_s *)pkt;

  /* fill the request */
  rpcb.id = call->id = htonl(server->rpc_id++);
  call->type = htonl(MSG_CALL);
  call->rpcvers = htonl(2);
  call->prog = htonl(program);
  call->vers = htonl(version);
  call->proc = htonl(procedure);

  /* copy request body */
  p = (void *)(call + 1);
  memcpy(p, *data, sz);

  /* fill RPC block */
  semaphore_init(&rpcb.sem, 0);
  rpcb.data = NULL;
  if (!rpcb_push(&server->rpc_blocks, &rpcb))
    {
      mem_free(pkt);
      return -ENOMEM;
    }

 retry:

  /* send the packet */
  switch (program)
    {
      case PROGRAM_PORTMAP:
	dest = &server->portmap;
	break;
      case PROGRAM_MOUNTD:
	dest = &server->mountd;
	break;
      case PROGRAM_NFSD:
	dest = &server->nfsd;
	break;
      default:
	assert(!"bad RPC program id");
    }

  if (udp_send(server->conn, dest, pkt, sizeof (struct rpc_call_s) + sz) < 0)
    {
      rpcb_remove(&server->rpc_blocks, &rpcb);
      mem_free(pkt);

      return -EAGAIN;
    }

  /* start timeout */
  rpcb.timeout.delay = RPC_TIMEOUT;
  rpcb.timeout.callback = rpc_timeout;
  rpcb.timeout.pv = &rpcb;
  if (timer_add_event(&timer_ms, &rpcb.timeout))
    {
      rpcb_remove(&server->rpc_blocks, &rpcb);
      mem_free(pkt);

      return -ENOMEM;
    }

  /* wait reply */
  semaphore_take(&rpcb.sem, 1);

  /* get reply */
  reply = (struct rpc_reply_s *)rpcb.data;

  /* check for error */
  if (reply == NULL)
    {
      /* retry 3 times */
      if (tries++ < 3)
	goto retry;
      else
	{
	  printk("RPC timeout\n");

	  rpcb_remove(&server->rpc_blocks, &rpcb);
	  mem_free(pkt);

	  return -EAGAIN;
	}
    }

  rpcb_remove(&server->rpc_blocks, &rpcb);

  if (reply->rstatus || reply->astatus || reply->verifier)
    {
      mem_free(pkt);
      mem_free(reply);

      return -EINVAL;
    }

  mem_free(pkt);

  *data = rpcb.data;
  *size = rpcb.size;

  return 0;
}

/*
 * Initialize NFS connection by identifing remote ports using portmap.
 */

error_t		net_nfs_init(struct nfs_s	*server)
{
  static struct
  {
    uint32_t	mbz[4];
    uint32_t	program;
    uint32_t	version;
    uint32_t	protocol;
    uint32_t	mbz2;
  } mountd =
    {
      .mbz = { 0, 0, 0, 0 },
      .mbz2 = 0
    },
    nfsd =
    {
      .mbz = { 0, 0, 0, 0 },
      .mbz2 = 0
    };
  void		*req;
  size_t	size;
  uint32_t	*port;

  /* init RPC block list */
  rpcb_init(&server->rpc_blocks);

  /* set local port so UDP will determine a free port */
  IPV4_ADDR_SET(server->local.address, INADDR_ANY);
  server->local.port = htons(700) /* XXX */;

  /* fill the portmap address */
  memcpy(&server->portmap.address, &server->address, sizeof (struct net_addr_s));
  server->portmap.port = htons(111);

  /* generate a RPC identifier */
  server->rpc_id = timer_get_tick(&timer_ms);

  /* register local rpc socket */
  server->conn = NULL;
  if (udp_bind(&server->conn, &server->local, rpc_callback, server) < 0)
    return -1;

  /*
   * get mountd port.
   */

  mountd.program = htonl(PROGRAM_MOUNTD);
  mountd.version = htonl(1);
  mountd.protocol = htonl(IPPROTO_UDP);
  req = &mountd;
  size = sizeof (mountd);

  if (do_rpc(server, PROGRAM_PORTMAP, 2, PORTMAP_GETPORT, &req, &size))
    return -1;

  port = (uint32_t *)((struct rpc_reply_s *)req + 1);
  memcpy(&server->mountd.address, &server->address, sizeof (struct net_addr_s));
  server->mountd.port = htons(ntohl(*port));

  mem_free(req);

  /*
   * get nfsd port.
   */

  nfsd.program = htonl(PROGRAM_NFSD);
  nfsd.version = htonl(2);
  nfsd.protocol = htonl(IPPROTO_UDP);
  req = &nfsd;
  size = sizeof (nfsd);

  if (do_rpc(server, PROGRAM_PORTMAP, 2, PORTMAP_GETPORT, &req, &size))
    return -1;

  port = (uint32_t *)((struct rpc_reply_s *)req + 1);
  memcpy(&server->nfsd.address, &server->address, sizeof (struct net_addr_s));
  server->nfsd.port = htons(ntohl(*port));

  mem_free(req);

  return 0;
}

/*
 * Close a NFS connection, unmounting all exports.
 */

void			net_nfs_destroy(struct nfs_s	*server)
{
  net_nfs_umount_all(server);

  udp_close(server->conn);
  rpcb_destroy(&server->rpc_blocks);
}

/*
 * RPC to mount deamon
 */

static error_t		net_nfs_mountd(struct nfs_s	*server,
				       void		*call,
				       size_t		*size,
				       uint32_t		action,
				       void		**reply)
{
  struct nfs_auth_s	*auth;
  void			*buf;
  void			*req;

  /* allocate packet for the request */
  if ((buf = req = mem_alloc(sizeof (struct nfs_auth_s) + *size, (mem_scope_context))) == NULL)
    return -ENOMEM;

  auth = (struct nfs_auth_s *)req;

  /* insert auth info */
  auth->credential = htonl(1);
  auth->cred_len = htonl(20);
  auth->timestamp = 0;
  auth->hostname_len = 0;
  auth->uid = 0;
  auth->gid = 0;
  auth->aux_gid = 0;
  auth->verifier = 0;
  auth->verif_len = 0;

  memcpy(auth + 1, call, *size);
  *size += sizeof (struct nfs_auth_s);

  /* RPC */
  if (do_rpc(server, PROGRAM_MOUNTD, 1, action, &req, size))
    {
      mem_free(buf);

      return -EINVAL;
    }

  mem_free(buf);
  *reply = req;

  return 0;
}

/*
 * Mount an export.
 */

error_t			net_nfs_mount(struct nfs_s	*server,
				      const char	*path,
				      nfs_handle_t	root)
{
  struct nfs_dirop_s	*dir;
  struct nfs_status_s	*stat;
  struct rpc_reply_s	*reply;
  error_t		err;
  size_t		path_len;
  size_t		sz;

  path_len = strlen(path);
  sz = sizeof(uint32_t) + ALIGN_VALUE_UP(path_len, 4);

  if ((dir = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy path to the export */
  dir->path_len = htonl(path_len);
  if (path_len & 3)
    dir->path[path_len] = 0;
  memcpy(dir->path, path, path_len);

  /* call mountd */
  err = net_nfs_mountd(server, dir, &sz, MOUNT_MOUNT, (void *)&reply);
  mem_free(dir);

  if (err)
    return err;

  /* read error code */
  stat = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || ((err = stat->status)) ||
      sz < sizeof (uint32_t) + sizeof (nfs_handle_t))
    goto leave;

  /* read root handle */
  memcpy(root, stat->u.handle, sizeof (nfs_handle_t));

  mem_free(reply);
  return 0;

 leave:
  mem_free(reply);
  return err ? err : -EINVAL;
}

/*
 * Unmount an export.
 */

error_t			net_nfs_umount(struct nfs_s	*server,
				       const char	*path)
{
  struct nfs_dirop_s	*dir;
  struct rpc_reply_s	*reply;
  error_t		err;
  size_t		path_len;
  size_t		sz;

  path_len = strlen(path);
  sz = sizeof(uint32_t) + ALIGN_VALUE_UP(path_len, 4);

  if ((dir = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy path to the export */
  dir->path_len = htonl(path_len);
  if (path_len & 3)
    dir->path[path_len] = 0;
  memcpy(dir->path, path, path_len);

  /* call mountd */
  err = net_nfs_mountd(server, dir, &sz, MOUNT_UMOUNT, (void *)&reply);

  mem_free(dir);
  mem_free(reply);

  return 0;
}

/*
 * Unmount the mounted exports.
 */

error_t		net_nfs_umount_all(struct nfs_s	*server)
{
  struct rpc_reply_s	*reply;
  size_t		sz = 0;

  net_nfs_mountd(server, NULL, &sz, MOUNT_UMOUNTALL, (void *)&reply);

  mem_free(reply);

  return 0;
}

/*
 * RPC to nfs daemon
 */

static error_t		net_nfs_nfsd(struct nfs_s	*server,
				     void		*call,
				     size_t		*size,
				     uint32_t		action,
				     void		**reply)
{
  struct nfs_auth_s	*auth;
  void			*buf;
  void			*req;

  /* allocate packet for the request */
  if ((buf = req = mem_alloc(sizeof (struct nfs_auth_s) + *size, (mem_scope_context))) == NULL)
    return -ENOMEM;

  auth = (struct nfs_auth_s *)req;

  /* insert auth info */
  auth->credential = htonl(1);
  auth->cred_len = htonl(20);
  auth->timestamp = 0;
  auth->hostname_len = 0;
  auth->uid = htonl(server->uid);
  auth->gid = htonl(server->gid);
  auth->aux_gid = 0;
  auth->verifier = 0;
  auth->verif_len = 0;

  memcpy(auth + 1, call, *size);
  *size += sizeof (struct nfs_auth_s);

  /* RPC */
  if (do_rpc(server, PROGRAM_NFSD, 2, action, &req, size))
    {
      mem_free(buf);

      return -EINVAL;
    }

  mem_free(buf);
  *reply = req;

  return 0;
}

/*
 * Read data from a file.
 */

ssize_t				net_nfs_read(struct nfs_s	*server,
					     nfs_handle_t	handle,
					     void		*data,
					     off_t		offset,
					     size_t		size)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  size_t			sz2;
  error_t			err;

  if (size > 4096)
    size = 4096;

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t) + sizeof (struct nfs_read_s);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy handle & read info */
  memcpy(req->handle, handle, sizeof (nfs_handle_t));
  req->u.read.offset = htonl(offset);
  req->u.read.count = htonl(size);
  req->u.read.__unused = 0;

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, NFS_READ, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
      sz < sizeof (struct nfs_attr_s) + 2 * sizeof (uint32_t))
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  /* copy data */
  if ((sz2 = status->u.attr_data.len))
    {
      sz2 = ntohl(sz2);
      sz -= sizeof (struct nfs_attr_s) + 2 * sizeof (uint32_t);
      if (sz < sz2)
	sz2 = sz;
      if (sz2 > size)
	sz2 = size;
      memcpy(data, status->u.attr_data.data, sz2);
    }

  mem_free(reply);
  return sz2;
}

/*
 * Write data to a file.
 */

ssize_t		net_nfs_write(struct nfs_s	*server,
			      nfs_handle_t	handle,
			      void		*data,
			      off_t		offset,
			      size_t		size)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;

  if (size > 4096)
    size = 4096;

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t) + sizeof (struct nfs_write_s) + size;
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy handle & write info */
  memcpy(req->handle, handle, sizeof (nfs_handle_t));
  req->u.write.offset = htonl(offset);
  req->u.write.count = req->u.write.count2 = htonl(size);
  req->u.write.__unused = 0;
  /* copy data */
  memcpy(req->u.write.data, data, size);

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, NFS_WRITE, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
      sz < sizeof (struct nfs_attr_s) + 2 * sizeof (uint32_t))
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  mem_free(reply);
  return size;
}


/*
 * Look for a file.
 */

error_t				net_nfs_lookup(struct nfs_s		*server,
					       const char		*path,
					       nfs_handle_t		directory,
					       nfs_handle_t		handle,
					       struct nfs_attr_s	*stat)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;
  size_t			path_len;

  path_len = strlen(path);

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t) + sizeof (uint32_t) + ALIGN_VALUE_UP(path_len, 4);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy root & path to the entity */
  memcpy(req->handle, directory, sizeof (nfs_handle_t));
  req->u.dirop.path_len = htonl(path_len);
  memcpy(req->u.dirop.path, path, path_len);
  if (path_len & 3)
    req->u.dirop.path[path_len] = 0;

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, NFS_LOOKUP, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
      sz < sizeof (uint32_t) + sizeof (struct nfs_attr_s) + sizeof (nfs_handle_t))
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  /* copy stat if neeeded */
  if (stat != NULL)
    {
      memcpy(stat, &status->u.handle_attr.attr, sizeof (struct nfs_attr_s));
      attr_endian(stat);
    }

  /* copy handle */
  memcpy(handle, status->u.handle_attr.handle, sizeof (nfs_handle_t));

  mem_free(reply);
  return 0;
}

/*
 * Get filesystem statistics
 */

error_t				net_nfs_statfs(struct nfs_s		*server,
					       nfs_handle_t		root,
					       struct nfs_statfs_s	*stats)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy root & path to the entity */
  memcpy(req->handle, root, sizeof (nfs_handle_t));

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, NFS_STATFS, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
      sz < sizeof (uint32_t) + sizeof (struct nfs_statfs_s))
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  memcpy(stats, &status->u.statfs, sizeof (struct nfs_statfs_s));
  stat_endian(stats);

  mem_free(reply);
  return 0;
}

/*
 * Get attributes of a file
 */

error_t				net_nfs_getattr(struct nfs_s		*server,
						nfs_handle_t		handle,
						struct nfs_attr_s	*stat)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy handle */
  memcpy(req->handle, handle, sizeof (nfs_handle_t));

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, NFS_GETATTR, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
      sz < sizeof (uint32_t) + sizeof (struct nfs_attr_s))
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  memcpy(stat, &status->u.attr, sizeof (struct nfs_attr_s));
  attr_endian(stat);

  mem_free(reply);
  return 0;
}

/*
 * Set attributes of a file
 */

error_t				net_nfs_setattr(struct nfs_s		*server,
						nfs_handle_t		handle,
						struct nfs_attr_s	*stat)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t) + sizeof (struct nfs_user_attr_s);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy handle & attributes */
  memcpy(req->handle, handle, sizeof (nfs_handle_t));
  req->u.sattr.mode = htonl(stat->mode);
  req->u.sattr.uid = htonl(stat->uid);
  req->u.sattr.gid = htonl(stat->gid);
  req->u.sattr.size = htonl(stat->size);
  req->u.sattr.atime.tv_sec = htonl(stat->atime.tv_sec);
  req->u.sattr.atime.tv_usec = htonl(stat->atime.tv_usec);
  req->u.sattr.mtime.tv_sec = htonl(stat->mtime.tv_sec);
  req->u.sattr.mtime.tv_usec = htonl(stat->mtime.tv_usec);

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, NFS_SETATTR, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
      sz < sizeof (uint32_t) + sizeof (struct nfs_attr_s))
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  memcpy(stat, &status->u.attr, sizeof (struct nfs_attr_s));
  attr_endian(stat);

  mem_free(reply);
  return 0;
}

/*
 * Create a new file or directory (specifying some attributes).
 */

error_t				net_nfs_create(struct nfs_s		*server,
					       nfs_handle_t		directory,
					       const char		*name,
					       struct nfs_attr_s	*stat,
					       nfs_handle_t		created,
					       bool_t			is_dir)
{
  struct nfs_request_handle_s	*req;
  struct nfs_user_attr_s	*sattr;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;
  size_t			path_len;

  path_len = strlen(name);

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t) + sizeof (uint32_t) + ALIGN_VALUE_UP(path_len, 4) +
    sizeof (struct nfs_user_attr_s);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy root & path to the entity */
  memcpy(req->handle, directory, sizeof (nfs_handle_t));
  memcpy(req->u.dirop.path, name, path_len);
  if (path_len & 3)
    req->u.dirop.path[path_len] = 0;
  req->u.dirop.path_len = htonl(path_len);
  path_len = ALIGN_VALUE_UP(path_len, 4);
  /* setup attributes */
  sattr = (struct nfs_user_attr_s *)(req->u.dirop.path + path_len);
  if (stat != NULL)
    {
      sattr->mode = htonl(stat->mode);
      sattr->uid = htonl(stat->uid);
      sattr->gid = htonl(stat->gid);
      sattr->size = htonl(stat->size);
      sattr->atime.tv_sec = htonl(stat->atime.tv_sec);
      sattr->atime.tv_usec = htonl(stat->atime.tv_usec);
      sattr->mtime.tv_sec = htonl(stat->mtime.tv_sec);
      sattr->mtime.tv_usec = htonl(stat->mtime.tv_usec);
    }
  else
    {
      uint32_t	ignore = htonl(-1);

      sattr->mode = ignore;
      sattr->uid = ignore;
      sattr->gid = ignore;
      sattr->size = ignore;
      sattr->atime.tv_sec = ignore;
      sattr->atime.tv_usec = ignore;
      sattr->mtime.tv_sec = ignore;
      sattr->mtime.tv_usec = ignore;
    }

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, is_dir ? NFS_MKDIR : NFS_CREATE, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
      sz < sizeof (uint32_t) + sizeof (struct nfs_attr_s) + sizeof (nfs_handle_t))
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  /* copy stat if neeeded */
  if (stat != NULL)
    {
      memcpy(stat, &status->u.handle_attr.attr, sizeof (struct nfs_attr_s));
      attr_endian(stat);
    }

  /* copy handle */
  memcpy(created, status->u.handle_attr.handle, sizeof (nfs_handle_t));

  mem_free(reply);
  return 0;
}

/*
 * Remove a file or directory
 */

error_t		net_nfs_remove(struct nfs_s		*server,
			       nfs_handle_t		directory,
			       const char		*name,
			       bool_t			is_dir)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;
  size_t			path_len;

  path_len = strlen(name);

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t) + sizeof (uint32_t) + ALIGN_VALUE_UP(path_len, 4);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy root & path to the entity */
  memcpy(req->handle, directory, sizeof (nfs_handle_t));
  req->u.dirop.path_len = htonl(path_len);
  memcpy(req->u.dirop.path, name, path_len);
  if (path_len & 3)
    req->u.dirop.path[path_len] = 0;

  /* call nfsd */
  err = net_nfs_nfsd(server, req, &sz, is_dir ? NFS_RMDIR : NFS_REMOVE, (void *)&reply);
  mem_free(req);

  if (err)
    return err;

  status = (struct nfs_status_s *)(reply + 1);

  if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK)
    {
      mem_free(reply);
      return err ? ntohl(err) : -EINVAL;
    }

  mem_free(reply);
  return 0;
}

/*
 * Readdir lists the contents of a directory.
 */

error_t		net_nfs_readdir(struct nfs_s	*server,
				nfs_handle_t	directory,
				nfs_readdir_t	callback,
				void		*pv)
{
  struct nfs_request_handle_s	*req;
  struct rpc_reply_s		*reply;
  struct nfs_status_s		*status;
  size_t			sz;
  error_t			err;
  uint8_t			*cookie;

  /* allocate packet for the request */
  sz = sizeof (nfs_handle_t) + sizeof (nfs_cookie_t) + sizeof (uint32_t);
  if ((req = mem_alloc(sz, (mem_scope_context))) == NULL)
    return -ENOMEM;

  /* copy handle */
  memcpy(req->handle, directory, sizeof (nfs_handle_t));
  memset(req->u.readdir.cookie, 0, sizeof (nfs_cookie_t));
  req->u.readdir.count = htonl(4096);

  while (1)
    {
      struct nfs_dirent_s	*ent;
      char			filename[MAXNAMLEN];
      uint32_t			*eof;

      /* call nfsd */
      err = net_nfs_nfsd(server, req, &sz, NFS_READDIR, (void *)&reply);

      if (err)
	{
	  mem_free(req);
	  return err;
	}

      status = (struct nfs_status_s *)(reply + 1);

      if (sz < sizeof (uint32_t) || (err = status->status) != NFS_OK ||
	  sz < sizeof (uint32_t) + sizeof (uint32_t))
	{
	  mem_free(reply);
	  mem_free(req);
	  return err ? ntohl(err) : -EINVAL;
	}

      eof = (uint32_t *)status->u.data;
      sz -= 2 * sizeof (uint32_t);

      /* list entries */
      for (ent = (struct nfs_dirent_s *)(eof + 1);
	   *eof;
	   ent = (struct nfs_dirent_s *)(eof + 1))
	{
	  if (sz < 3 * sizeof (uint32_t) + sizeof (nfs_cookie_t))
	    {
	      eof = NULL;
	      break;
	    }

	  ent->len = ntohl(ent->len);
	  memcpy(filename, ent->data, ent->len);
	  filename[ent->len] = 0;
	  if (callback(filename, pv))
	    break;
	  cookie = (uint8_t *)(&ent->data[ALIGN_VALUE_UP(ent->len, 4)]);
	  eof = (uint32_t *)(cookie + sizeof (nfs_cookie_t));
	  sz -= ((uint8_t *)eof - (uint8_t *)ent) + sizeof (uint32_t);
	}

      if (eof != NULL)
	{
	  eof++;

	  if (*eof)
	    break;

	  memcpy(req->u.readdir.cookie, ent, sizeof (nfs_cookie_t));
	}
      else
	break;
    }

  mem_free(req);
  mem_free(reply);
  return 0;
}
