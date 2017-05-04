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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

/**
 * @file
 * @module{Hexo}
 * @short Common error codes definitions
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include "types.h"

/** error code type */
typedef int_fast8_t		error_t;

/** unknown or undefined error */
#define EUNKNOWN	1

/** missing ot not found entry error */
#define ENOENT		2

/** ressource busy error */
#define EBUSY		3

/** no more memory available for the requested operation */
#define ENOMEM		4

/** invalid value */
#define EINVAL		5

/** deadlock detected */
#define EDEADLK		6

/** operation not permitted */
#define EPERM		7

/** operation not supported */
#define ENOTSUP		8

/** service temporarily unavailable */
#define EAGAIN		9

/** value out of range */
//#define ERANGE		10

/** io error */
#define EIO		11

/** end of data */
#define EEOF		12

/** bad data/inconsistency */
#define EBADDATA	13

/** operation canceled */
#define ECANCELED 14

/** operation canceled */
#define EACCES 15

/** File exists */
#define EEXISTS 17

/** Is a directory */
#define EISDIR 21

/** broken pipe */
#define EPIPE		32

/** Math argument out of domain of func */
#define EDOM        33
/** Math result not representable */
#define ERANGE      34

/** Function not implemented */
#define ENOSYS      38

/** address in use */
#define EADDRINUSE	40

/** address not available */
#define EADDRNOTAVAIL	41

/** destination address required */
#define EDESTADDRREQ	89

/** protocol or option not available */
#define ENOPROTOOPT	92

/** protocol not supported */
#define EPROTONOSUPPORT	93

/** operation not supported on transport endpoint */
#define EOPNOTSUPP	95

/** protocol family not supported */
#define EPFNOSUPPORT	96

/** address family not supported by protocol */
#define EAFNOSUPPORT	97

/** transport endpoint is already connected */
#define EISCONN		106

/** transport endpoint not connected */
#define ENOTCONN	107

/** cannot send after transport endpoint shutdown */
#define ESHUTDOWN	108

/** no route to host */
#define EHOSTUNREACH	113

C_HEADER_END

#endif

