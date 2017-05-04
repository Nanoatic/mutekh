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

#if !defined(NETINET_SOCKET_H) || defined(SOCKET_HEXO_H)
#error This file can not be included directly
#else

#define SOCKET_HEXO_H

/* Create a new socket */
socket_t	socket(int_fast32_t domain, int_fast32_t type, int_fast32_t protocol);

/* Give the socket FD the local address ADDR (which is LEN bytes long).  */
static inline __attribute__((always_inline)) _BIND(bind)
{
  return fd->f->bind(fd, addr, len);
}

/* Put the local address of FD into *ADDR and its length in *LEN.  */
static inline __attribute__((always_inline)) _GETSOCKNAME(getsockname)
{
  return fd->f->getsockname(fd, addr, len);
}

/* Open a connection on socket FD to peer at ADDR (which LEN bytes long).
   For connectionless socket types, just set the default address to send to
   and the only address from which to accept transmissions.
   Return 0 on success, -1 for errors.

   This function is a cancellation point and therefore not marked with
  .  */
static inline __attribute__((always_inline)) _CONNECT(connect)
{
  return fd->f->connect(fd, addr, len);
}

/* Put the address of the peer connected to socket FD into *ADDR
   (which is *LEN bytes long), and its actual length into *LEN.  */
static inline __attribute__((always_inline)) _GETPEERNAME(getpeername)
{
  return fd->f->getpeername(fd, addr, len);
}

/* Send N bytes of BUF on socket FD to peer at address ADDR (which is
   ADDR_LEN bytes long).  Returns the number sent, or -1 for errors.

   This function is a cancellation point and therefore not marked with
  .  */
static inline __attribute__((always_inline)) _SENDMSG(sendmsg)
{
  return fd->f->sendmsg(fd, message, flags);
}

static inline __attribute__((always_inline)) _SENDTO(sendto)
{
  struct msghdr	msg;
  struct iovec	v;

  v.iov_base = (void *)buf;
  v.iov_len = n;
  msg.msg_name = addr;
  msg.msg_namelen = addr_len;
  msg.msg_iov = &v;
  msg.msg_iovlen = 1;
  msg.msg_controllen = 0;
  return fd->f->sendmsg(fd, &msg, flags);
}

static inline __attribute__((always_inline)) _SEND(send)
{
  struct msghdr	msg;
  struct iovec	v;

  v.iov_base = (void *)buf;
  v.iov_len = n;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &v;
  msg.msg_iovlen = 1;
  msg.msg_controllen = 0;
  return fd->f->sendmsg(fd, &msg, flags);
}

/* Read N bytes into BUF through socket FD.
   If ADDR is not NULL, fill in *ADDR_LEN bytes of it with tha address of
   the sender, and store the actual size of the address in *ADDR_LEN.
   Returns the number of bytes read or -1 for errors.

   This function is a cancellation point and therefore not marked with
  .  */
static inline __attribute__((always_inline)) _RECVMSG(recvmsg)
{
  return fd->f->recvmsg(fd, message, flags);
}

static inline __attribute__((always_inline)) _RECVFROM(recvfrom)
{
  struct msghdr	msg;
  struct iovec	v;
  ssize_t	ret;

  v.iov_base = buf;
  v.iov_len = n;
  msg.msg_name = addr;
  msg.msg_namelen = *addr_len;
  msg.msg_iov = &v;
  msg.msg_iovlen = 1;
  msg.msg_controllen = 0;
  ret = fd->f->recvmsg(fd, &msg, flags);
  if (!ret)
    *addr_len = msg.msg_namelen;
  return ret;
}

static inline __attribute__((always_inline)) _RECV(recv)
{
  struct msghdr	msg;
  struct iovec	v;

  v.iov_base = buf;
  v.iov_len = n;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &v;
  msg.msg_iovlen = 1;
  msg.msg_controllen = 0;
  return fd->f->recvmsg(fd, &msg, flags);
}

/* Put the current value for socket FD's option OPTNAME at protocol level LEVEL
   into OPTVAL (which is *OPTLEN bytes long), and set *OPTLEN to the value's
   actual length.  Returns 0 on success, -1 for errors.  */
static inline __attribute__((always_inline)) _GETSOCKOPT(getsockopt)
{
  return fd->f->getsockopt(fd, level, optname, optval, optlen);
}

/* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */
static inline __attribute__((always_inline)) _SETSOCKOPT(setsockopt)
{
  return fd->f->setsockopt(fd, level, optname, optval, optlen);
}

/* Prepare to accept connections on socket FD.
   N connection requests will be queued before further requests are refused.
   Returns 0 on success, -1 for errors.  */
static inline __attribute__((always_inline)) _LISTEN(listen)
{
  return fd->f->listen(fd, n);
}

/* Await a connection on socket FD.
   When a connection arrives, open a new socket to communicate with it,
   set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
   peer and *ADDR_LEN to the address's actual length, and return the
   new socket's descriptor, or -1 for errors.

   This function is a cancellation point and therefore not marked with
  .  */
static inline __attribute__((always_inline)) _ACCEPT(accept)
{
  return fd->f->accept(fd, addr, addr_len);
}

/* Shut down all or part of the connection open on socket FD.
   HOW determines what to shut down:
     SHUT_RD   = No more receptions;
     SHUT_WR   = No more transmissions;
     SHUT_RDWR = No more receptions or transmissions.
   Returns 0 on success, -1 for errors.  */
static inline __attribute__((always_inline)) _SHUTDOWN(shutdown)
{
  return fd->f->shutdown(fd, how);
}

#endif
