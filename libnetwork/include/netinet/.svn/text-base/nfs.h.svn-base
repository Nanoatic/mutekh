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

#ifndef NETINET_NFS_H_
#define NETINET_NFS_H_

#ifndef CONFIG_NETWORK_NFS
# warning NFS support is not enabled in configuration file
#endif

#include <hexo/types.h>
#include <netinet/libudp.h>
#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_hashlist.h>

#include <semaphore.h>
#include <mutek/timer.h>

/*
 * RPC timeout.
 */

#define RPC_TIMEOUT	3000 /* 3 seconds */

/*
 * Programs ID.
 */

#define PROGRAM_PORTMAP	100000
#define PROGRAM_MOUNTD	100005
#define PROGRAM_NFSD	100003

/*
 * Message type.
 */

#define MSG_CALL	0
#define MSG_REPLY	1

/*
 * Portmap calls.
 */

#define PORTMAP_GETPORT	3

/*
 * mountd calls.
 */

#define MOUNT_NULL	0
#define MOUNT_MOUNT	1
#define MOUNT_DUMP	2
#define MOUNT_UMOUNT	3
#define MOUNT_UMOUNTALL	4
#define MOUNT_EXPORT	5

/*
 * nfsd calls
 */

#define NFS_NULL	0
#define NFS_GETATTR	1
#define NFS_SETATTR	2
#define NFS_ROOT	3
#define NFS_LOOKUP	4
#define NFS_READLINK	5
#define NFS_READ	6
#define NFS_WRITECACHE	7
#define NFS_WRITE	8
#define NFS_CREATE	9
#define NFS_REMOVE	10
#define NFS_RENAME	11
#define NFS_LINK	12
#define NFS_SYMLINK	13
#define NFS_MKDIR	14
#define NFS_RMDIR	15
#define NFS_READDIR	16
#define NFS_STATFS	17

/*
 * File handles.
 */

#define FHSIZE	32

typedef uint8_t nfs_handle_t[FHSIZE];

/*
 * Misc.
 */

#define MAXNAMLEN	255
#define MAXPATHLEN	1024
#define MAXDATA		8192
#define NFS_COOKIESIZE	4

/*
 * NFS file types.
 */

#define NFNON	0
#define NFREG	1
#define NFDIR	2
#define NFBLK	3
#define NFCHR	4
#define NFLNK	5

/*
 * NFS errors.
 */

#define NFS_OK			0
#define NFSERR_PERM		1
#define NFSERR_NOENT		2
#define NFSERR_IO		5
#define NFSERR_NXIO		6
#define NFSERR_ACCES		13
#define NFSERR_EXIST		17
#define NFSERR_NODEV		19
#define NFSERR_NOTDIR		20
#define NFSERR_ISDIR		21
#define NFSERR_FBIG		27
#define NFSERR_NOSPC		28
#define NFSERR_ROFS		30
#define NFSERR_NAMETOOLONG	63
#define NFSERR_NOTEMPTY		66
#define NFSERR_DQUOT		69
#define NFSERR_STALE		70
#define NFSERR_WFLUSH		99

/*
 * RPC block.
 */

struct					rpcb_s
{
  uint_fast32_t				id;
  struct timer_event_s			timeout;
  void					*data;
  size_t				size;
  struct semaphore_s					sem;

  CONTAINER_ENTRY_TYPE(HASHLIST)	list_entry;
};

CONTAINER_TYPE(rpcb, HASHLIST, struct rpcb_s, list_entry, 64);
CONTAINER_KEY_TYPE(rpcb, PTR, SCALAR, id);

/*
 * NFS connection descriptor.
 */

struct			nfs_s
{
  struct net_udp_desc_s	*conn;
  struct net_udp_addr_s	local;
  struct net_addr_s	address;	/* server address */
  struct net_udp_addr_s	portmap;	/* portmap server address */
  struct net_udp_addr_s	mountd;		/* mountd server address */
  struct net_udp_addr_s	nfsd;		/* nfsd server address */
  uint_fast32_t		rpc_id;		/* rpc sequence id */
  uint_fast32_t		uid;
  uint_fast32_t		gid;

  rpcb_root_t		rpc_blocks;
};

/*
 * NFS auth.
 */

struct			nfs_auth_s
{
  uint32_t		credential;
  uint32_t		cred_len;
  uint32_t		timestamp;
  uint32_t		hostname_len;
  uint32_t		uid;
  uint32_t		gid;
  uint32_t		aux_gid;
  uint32_t		verifier;
  uint32_t		verif_len;
} __attribute__((packed));

/*
 * NFS time value.
 */

struct			nfs_timeval_s
{
  uint32_t		tv_sec;
  uint32_t		tv_usec;
} __attribute__((packed));

/*
 * NFS file attributes
 */

struct			nfs_attr_s
{
  uint32_t		ftype;
  uint32_t		mode;
  uint32_t		nlink;
  uint32_t		uid;
  uint32_t		gid;
  uint32_t		size;
  uint32_t		blocksize;
  uint32_t		rdev;
  uint32_t		blocks;
  uint32_t		fsid;
  uint32_t		fileid;
  struct nfs_timeval_s	atime;
  struct nfs_timeval_s	mtime;
  struct nfs_timeval_s	ctime;
} __attribute__((packed));

/*
 * NFS file attributes (user)
 */

struct			nfs_user_attr_s
{
  uint32_t		mode;
  uint32_t		uid;
  uint32_t		gid;
  uint32_t		size;
  struct nfs_timeval_s	atime;
  struct nfs_timeval_s	mtime;
} __attribute__((packed));

/*
 * NFS status with handle and attributes
 */

struct			nfs_handle_attr_s
{
  nfs_handle_t		handle;
  struct nfs_attr_s	attr;
} __attribute__((packed));

/*
 * NFS status with attributes and data
 */

struct			nfs_attr_data_s
{
  struct nfs_attr_s	attr;
  uint32_t		len;
  uint8_t		data[1];
} __attribute__((packed));

/*
 * NFS stat filesystem
 */

struct			nfs_statfs_s
{
  uint32_t		transfer_unit;
  uint32_t		block_size;
  uint32_t		blocks;
  uint32_t		blocks_free;
  uint32_t		blocks_avail;
} __attribute__((packed));

/*
 * NFS dir entry
 */

struct			nfs_dirent_s
{
  uint32_t		fileid;
  uint32_t		len;
  char			data[1];
} __attribute__((packed));

/*
 * NFS status union
 */

struct				nfs_status_s
{
  uint32_t			status;
  union
  {
    struct nfs_attr_s		attr;
    nfs_handle_t		handle;
    struct nfs_attr_data_s	attr_data;
    struct nfs_handle_attr_s	handle_attr;
    struct nfs_statfs_s		statfs;
    uint8_t			data[1];
  } u;
} __attribute__((packed));

/*
 * NFS directory operation
 */

struct			nfs_dirop_s
{
  uint32_t		path_len;
  uint8_t		path[1];
} __attribute__((packed));

/*
 * NFS read request
 */

struct			nfs_read_s
{
  uint32_t		offset;
  uint32_t		count;
  uint32_t		__unused;
} __attribute__((packed));

/*
 * NFS write request
 */

struct			nfs_write_s
{
  uint32_t		__unused;
  uint32_t		offset;
  uint32_t		count;
  uint32_t		count2;
  uint8_t		data[1];
} __attribute__((packed));

/*
 * NFS readdir
 */

typedef uint8_t nfs_cookie_t[NFS_COOKIESIZE];

struct			nfs_readdir_s
{
  nfs_cookie_t		cookie;
  uint32_t		count;
} __attribute__((packed));

/*
 * NFS request with handle
 */

struct				nfs_request_handle_s
{
  nfs_handle_t			handle;
  union
  {
    struct nfs_user_attr_s	sattr;
    struct nfs_dirop_s		dirop;
    struct nfs_read_s		read;
    struct nfs_write_s		write;
    struct nfs_readdir_s	readdir;
  } u;
} __attribute__((packed));

/*
 * RPC call.
 */

struct			rpc_call_s
{
  uint32_t		id;
  uint32_t		type;
  uint32_t		rpcvers;
  uint32_t		prog;
  uint32_t		vers;
  uint32_t		proc;
} __attribute__((packed));

/*
 * RPC reply.
 */

struct			rpc_reply_s
{
  uint32_t		id;
  uint32_t		type;
  uint32_t		rstatus;
  uint32_t		verifier;
  uint32_t		v2;
  uint32_t		astatus;
} __attribute__((packed));

/*
 * Prototypes.
 */

error_t		net_nfs_init(struct nfs_s	*server);
void		net_nfs_destroy(struct nfs_s	*server);

/* mount and umount operations */
error_t		net_nfs_mount(struct nfs_s	*server,
			      const char	*path,
			      nfs_handle_t	root);
error_t		net_nfs_umount(struct nfs_s	*server,
			       const char	*path);
error_t		net_nfs_umount_all(struct nfs_s	*server);

/* lookup & attributes */
error_t		net_nfs_lookup(struct nfs_s		*server,
			       const char		*path,
			       nfs_handle_t		directory,
			       nfs_handle_t		handle,
			       struct nfs_attr_s	*stat);
error_t		net_nfs_getattr(struct nfs_s		*server,
				nfs_handle_t		handle,
				struct nfs_attr_s	*stat);
error_t		net_nfs_setattr(struct nfs_s		*server,
				nfs_handle_t		handle,
				struct nfs_attr_s	*stat);

/* read/write operations */
ssize_t		net_nfs_read(struct nfs_s	*server,
			     nfs_handle_t	handle,
			     void		*data,
			     off_t		offset,
			     size_t		size);

ssize_t		net_nfs_write(struct nfs_s	*server,
			      nfs_handle_t	handle,
			      void		*data,
			      off_t		offset,
			      size_t		size);

/* file creation, removing, renaming, links */
error_t		net_nfs_create(struct nfs_s		*server,
			       nfs_handle_t		directory,
			       const char		*name,
			       struct nfs_attr_s	*stat,
			       nfs_handle_t		created,
			       bool_t			is_dir);

static inline error_t nfs_creat(struct nfs_s		*server,
				nfs_handle_t		directory,
				const char		*name,
				struct nfs_attr_s	*stat,
				nfs_handle_t		created)
{
  return net_nfs_create(server, directory, name, stat, created, 0);
}

static inline error_t nfs_mkdir(struct nfs_s		*server,
				nfs_handle_t		directory,
				const char		*name,
				struct nfs_attr_s	*stat,
				nfs_handle_t		created)
{
  return net_nfs_create(server, directory, name, stat, created, 1);
}

error_t		net_nfs_remove(struct nfs_s		*server,
			       nfs_handle_t		directory,
			       const char		*name,
			       bool_t			is_dir);

static inline error_t nfs_unlink(struct nfs_s	*server,
				 nfs_handle_t	directory,
				 const char	*name)
{
  return net_nfs_remove(server, directory, name, 0);
}

static inline error_t nfs_rmdir(struct nfs_s	*server,
				nfs_handle_t	directory,
				const char	*name)
{
  return net_nfs_remove(server, directory, name, 1);
}

/* XXX readlink, link, symlink */

/* readdir */
#define NFS_READDIR_CB(f)	bool_t	(f)(const char	*filename,	\
					    void	*pv)

typedef NFS_READDIR_CB(nfs_readdir_t);

error_t		net_nfs_readdir(struct nfs_s	*server,
				nfs_handle_t	directory,
				nfs_readdir_t	callback,
				void		*pv);

/* statfs */
error_t		net_nfs_statfs(struct nfs_s		*server,
			       nfs_handle_t		root,
			       struct nfs_statfs_s	*stats);

#endif
