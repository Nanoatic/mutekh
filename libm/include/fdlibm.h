
/* @(#)fdlibm.h 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#ifndef __FDLIBM_H_
#define __FDLIBM_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/types.h>

/**
  @file
  @module {Math library}
  @short Fdlibm header

  Developed at SunPro, a Sun Microsystems, Inc. business.
  Permission to use, copy, modify, and distribute this
  software is freely granted, provided that this notice 
  is preserved.
 */

#define	__P(p)	p

/** @this is ANSI/POSIX */
extern int32_t signgam;

/** @this defines exterm values */
#define	MAXFLOAT	((float)3.40282346638528860e+38)
#define	HUGE		MAXFLOAT

/** @multiple @hidden */
enum fdversion {fdlibm_ieee = -1, fdlibm_svid, fdlibm_xopen, fdlibm_posix};
#define _LIB_VERSION_TYPE enum fdversion
#define _LIB_VERSION _fdlib_version  

/** @multiple @hidden if global variable _LIB_VERSION is not desirable, one may 
 * change the following to be a constant by: 
 *	#define _LIB_VERSION_TYPE const enum version
 * In that case, after one initializes the value _LIB_VERSION (see
 * s_lib_version.c) during compile time, it cannot be modified
 * in the middle of a program
 */ 
extern  _LIB_VERSION_TYPE  _LIB_VERSION;

#define _IEEE_  fdlibm_ieee
#define _SVID_  fdlibm_svid
#define _XOPEN_ fdlibm_xopen
#define _POSIX_ fdlibm_posix

/** @internal */
struct exception {
	int32_t type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

/** @this sets X_TLOSS to pi*2**52 */
#define X_TLOSS		1.41484755040568800000e+16 

/** @multiple */
#define	DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define	UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6

/**
 * @multiple
 * @this is an ANSI/POSIX math function
 */
extern double acos __P((double));
extern double asin __P((double));
extern double atan __P((double));
extern double atan2 __P((double, double));
extern double cos __P((double));
extern double sin __P((double));
extern double tan __P((double));

extern double cosh __P((double));
extern double sinh __P((double));
extern double tanh __P((double));

extern double exp __P((double));
extern double frexp __P((double, int32_t *));
extern double ldexp __P((double, int32_t));
extern double log __P((double));
extern double log10 __P((double));
extern double modf __P((double, double *));

extern double pow __P((double, double));
extern double sqrt __P((double));

extern double ceil __P((double));
extern double fabs __P((double));
extern double floor __P((double));
extern double fmod __P((double, double));

extern double erf __P((double));
extern double erfc __P((double));
extern double gamma __P((double));
extern double hypot __P((double, double));
extern int32_t isnan __P((double));
extern int32_t finite __P((double));
extern double j0 __P((double));
extern double j1 __P((double));
extern double jn __P((int32_t, double));
extern double lgamma __P((double));
extern double y0 __P((double));
extern double y1 __P((double));
extern double yn __P((int32_t, double));

extern double acosh __P((double));
extern double asinh __P((double));
extern double atanh __P((double));
extern double cbrt __P((double));
extern double logb __P((double));
extern double nextafter __P((double, double));
extern double remainder __P((double, double));
#ifdef _SCALB_INT
extern double scalb __P((double, int32_t));
#else
extern double scalb __P((double, double));
#endif

extern int32_t matherr __P((struct exception *));

/**
 * @this is IEEE Test Vector
 */
extern double significand __P((double));

/**
 * @multiple
 * @this is callable from C, intended to support IEEE arithmetic.
 */
extern double copysign __P((double, double));
extern int32_t ilogb __P((double));
extern double rint __P((double));
extern double scalbn __P((double, int32_t));

/**
 * @multiple
 * BSD math library entry point
 */
extern double expm1 __P((double));
extern double log1p __P((double));

/**
 * @multiple @this is a reentrant version of gamma & lgamma; passes
 * signgam back by reference as the second argument; user must
 * allocate space for signgam.
 */
#ifdef _REENTRANT
extern double gamma_r __P((double, int32_t *));
extern double lgamma_r __P((double, int32_t *));
#endif	/* _REENTRANT */

/** @internal @multiple @this is an IEEE style elementary function */
extern double __ieee754_sqrt __P((double));			
extern double __ieee754_acos __P((double));			
extern double __ieee754_acosh __P((double));			
extern double __ieee754_log __P((double));			
extern double __ieee754_atanh __P((double));			
extern double __ieee754_asin __P((double));			
extern double __ieee754_atan2 __P((double,double));			
extern double __ieee754_exp __P((double));
extern double __ieee754_cosh __P((double));
extern double __ieee754_fmod __P((double,double));
extern double __ieee754_pow __P((double,double));
extern double __ieee754_lgamma_r __P((double,int32_t *));
extern double __ieee754_gamma_r __P((double,int32_t *));
extern double __ieee754_lgamma __P((double));
extern double __ieee754_gamma __P((double));
extern double __ieee754_log10 __P((double));
extern double __ieee754_sinh __P((double));
extern double __ieee754_hypot __P((double,double));
extern double __ieee754_j0 __P((double));
extern double __ieee754_j1 __P((double));
extern double __ieee754_y0 __P((double));
extern double __ieee754_y1 __P((double));
extern double __ieee754_jn __P((int32_t,double));
extern double __ieee754_yn __P((int32_t,double));
extern double __ieee754_remainder __P((double,double));
extern int32_t    __ieee754_rem_pio2 __P((double,double*));
#ifdef _SCALB_INT
extern double __ieee754_scalb __P((double,int32_t));
#else
extern double __ieee754_scalb __P((double,double));
#endif

/** @internal @multiple @this is a fdlibm kernel function */
extern double __kernel_standard __P((double,double,int32_t));	
extern double __kernel_sin __P((double,double,int32_t));
extern double __kernel_cos __P((double,double));
extern double __kernel_tan __P((double,double,int32_t));
extern int32_t    __kernel_rem_pio2 __P((double*,double*,int32_t,int32_t,int32_t,const int32_t*));

C_HEADER_END

#endif
