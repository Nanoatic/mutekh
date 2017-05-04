
#include <string.h>
#include <errno.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

error_t errno = 0;

static const char *const strerror_tab[] = {
	[0] = "Success",
	[EUNKNOWN] = "Unknown or undefined error",
	[ENOENT] = "Missing or not found entry error",
	[EBUSY] = "Ressource busy error",
	[ENOMEM] = "No more memory available for the requested operation",
	[EINVAL] = "Invalid value",
	[EDEADLK] = "Deadlock detected",
	[EPERM] = "Operation not permitted",
	[ENOTSUP] = "Operation not supported",
	[EAGAIN] = "Service temporarily unavailable",
	[ERANGE] = "Value out of range",
	[ECANCELED] = "Operation has been canceled",
	[EACCES] = "Access denied",
	[EIO] = "Io error",
	[EEOF] = "End of data",
	[EEXISTS] = "File exists",
	[EISDIR] = "Is a directory",
	[EPIPE] = "Broken pipe",
	[EDOM] = "Math argument out of domain of func",
	[ERANGE] = "Math result not representable",
	[ENOSYS] = "Function not implemented",
	[EADDRINUSE] = "Address in use",
	[EADDRNOTAVAIL] = "Address not available",
	[EDESTADDRREQ] = "Destination address required",
	[ENOPROTOOPT] = "Protocol or option not available",
	[EPROTONOSUPPORT] = "Protocol not supported",
	[EOPNOTSUPP] = "Operation not supported on transport endpoint",
	[EPFNOSUPPORT] = "Protocol family not supported",
	[EAFNOSUPPORT] = "Address family not supported by protocol",
	[EISCONN] = "Transport endpoint is already connected",
	[ENOTCONN] = "Transport endpoint not connected",
	[ESHUTDOWN] = "Cannot send after transport endpoint shutdown",
	[EHOSTUNREACH] = "No route to host",
	
};

const char *strerror(error_t errnum)
{
	if ( errnum < 0 )
		errnum = -errnum;
	if ( errnum < ARRAY_SIZE(strerror_tab) && strerror_tab[errnum] != NULL )
		return strerror_tab[errnum];
	return "Unknown error";
}

