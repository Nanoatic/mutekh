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
  @file
  @module {Math library}
  @short Standard math header

  This header only defines some macro to use the gcc builtin math
  functions.
*/

#ifndef MATH_H_
#define MATH_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#if defined(CONFIG_LIBM)
# include <fdlibm.h>
#endif
/** Mathematical constant e */
#define M_E        2.7182818284590452354   
/** Mathematical constant log_2 e */
#define M_LOG2E    1.4426950408889634074   
/** Mathematical constant log_10 e */
#define M_LOG10E   0.43429448190325182765  
/** Mathematical constant log_e 2 */
#define M_LN2      0.69314718055994530942  
/** Mathematical constant log_e 10 */
#define M_LN10     2.30258509299404568402  
/** Mathematical constant pi */
#define M_PI       3.14159265358979323846  
/** Mathematical constant pi/2 */
#define M_PI_2     1.57079632679489661923  
/** Mathematical constant pi/4 */
#define M_PI_4     0.78539816339744830962  
/** Mathematical constant 1/pi */
#define M_1_PI     0.31830988618379067154  
/** Mathematical constant 2/pi */
#define M_2_PI     0.63661977236758134308  
/** Mathematical constant 2/sqrt(pi) */
#define M_2_SQRTPI 1.12837916709551257390  
/** Mathematical constant sqrt(2) */
#define M_SQRT2    1.41421356237309504880  
/** Mathematical constant 1/sqrt(2) */
#define M_SQRT1_2  0.70710678118654752440  

/** @multiple @this enables use of associated gcc builtin function */
#define acoshf __builtin_acoshf
#define acoshl __builtin_acoshl
#define acosh __builtin_acosh
#define asinhf __builtin_asinhf
#define asinhl __builtin_asinhl
#define asinh __builtin_asinh
#define atanhf __builtin_atanhf
#define atanhl __builtin_atanhl
#define atanh __builtin_atanh
#define cabsf __builtin_cabsf
#define cabsl __builtin_cabsl
#define cabs __builtin_cabs
#define cacosf __builtin_cacosf
#define cacoshf __builtin_cacoshf
#define cacoshl __builtin_cacoshl
#define cacosh __builtin_cacosh
#define cacosl __builtin_cacosl
#define cacos __builtin_cacos
#define cargf __builtin_cargf
#define cargl __builtin_cargl
#define carg __builtin_carg
#define casinf __builtin_casinf
#define casinhf __builtin_casinhf
#define casinhl __builtin_casinhl
#define casinh __builtin_casinh
#define casinl __builtin_casinl
#define casin __builtin_casin
#define catanf __builtin_catanf
#define catanhf __builtin_catanhf
#define catanhl __builtin_catanhl
#define catanh __builtin_catanh
#define catanl __builtin_catanl
#define catan __builtin_catan
#define cbrtf __builtin_cbrtf
#define cbrtl __builtin_cbrtl
#define cbrt __builtin_cbrt
#define ccosf __builtin_ccosf
#define ccoshf __builtin_ccoshf
#define ccoshl __builtin_ccoshl
#define ccosh __builtin_ccosh
#define ccosl __builtin_ccosl
#define ccos __builtin_ccos
#define cexpf __builtin_cexpf
#define cexpl __builtin_cexpl
#define cexp __builtin_cexp
#define cimagf __builtin_cimagf
#define cimagl __builtin_cimagl
#define cimag __builtin_cimag
#define clogf __builtin_clogf
#define clogl __builtin_clogl
#define clog __builtin_clog
#define conjf __builtin_conjf
#define conjl __builtin_conjl
#define conj __builtin_conj
#define copysignf __builtin_copysignf
#define copysignl __builtin_copysignl
#define copysign __builtin_copysign
#define cpowf __builtin_cpowf
#define cpowl __builtin_cpowl
#define cpow __builtin_cpow
#define cprojf __builtin_cprojf
#define cprojl __builtin_cprojl
#define cproj __builtin_cproj
#define crealf __builtin_crealf
#define creall __builtin_creall
#define creal __builtin_creal
#define csinf __builtin_csinf
#define csinhf __builtin_csinhf
#define csinhl __builtin_csinhl
#define csinh __builtin_csinh
#define csinl __builtin_csinl
#define csin __builtin_csin
#define csqrtf __builtin_csqrtf
#define csqrtl __builtin_csqrtl
#define csqrt __builtin_csqrt
#define ctanf __builtin_ctanf
#define ctanhf __builtin_ctanhf
#define ctanhl __builtin_ctanhl
#define ctanh __builtin_ctanh
#define ctanl __builtin_ctanl
#define ctan __builtin_ctan
#define erfcf __builtin_erfcf
#define erfcl __builtin_erfcl
#define erfc __builtin_erfc
#define erff __builtin_erff
#define erfl __builtin_erfl
#define erf __builtin_erf
#define exp2f __builtin_exp2f
#define exp2l __builtin_exp2l
#define exp2 __builtin_exp2
#define expm1f __builtin_expm1f
#define expm1l __builtin_expm1l
#define expm1 __builtin_expm1
#define fdimf __builtin_fdimf
#define fdiml __builtin_fdiml
#define fdim __builtin_fdim
#define fmaf __builtin_fmaf
#define fmal __builtin_fmal
#define fmaxf __builtin_fmaxf
#define fmaxl __builtin_fmaxl
#define fmax __builtin_fmax
#define fma __builtin_fma
#define fminf __builtin_fminf
#define fminl __builtin_fminl
#define fmin __builtin_fmin
#define hypotf __builtin_hypotf
#define hypotl __builtin_hypotl
#define hypot __builtin_hypot
#define ilogbf __builtin_ilogbf
#define ilogbl __builtin_ilogbl
#define ilogb __builtin_ilogb
#define imaxabs __builtin_imaxabs
#define isblank __builtin_isblank
#define iswblank __builtin_iswblank
#define lgammaf __builtin_lgammaf
#define lgammal __builtin_lgammal
#define lgamma __builtin_lgamma
//#define llabs __builtin_llabs
#define llrintf __builtin_llrintf
#define llrintl __builtin_llrintl
#define llrint __builtin_llrint
#define llroundf __builtin_llroundf
#define llroundl __builtin_llroundl
#define llround __builtin_llround
#define log1pf __builtin_log1pf
#define log1pl __builtin_log1pl
#define log1p __builtin_log1p
#define log2f __builtin_log2f
#define log2l __builtin_log2l
#define log2 __builtin_log2
#define logbf __builtin_logbf
#define logbl __builtin_logbl
#define logb __builtin_logb
#define lrintf __builtin_lrintf
#define lrintl __builtin_lrintl
#define lrint __builtin_lrint
#define lroundf __builtin_lroundf
#define lroundl __builtin_lroundl
#define lround __builtin_lround
#define nearbyintf __builtin_nearbyintf
#define nearbyintl __builtin_nearbyintl
#define nearbyint __builtin_nearbyint
#define nextafterf __builtin_nextafterf
#define nextafterl __builtin_nextafterl
#define nextafter __builtin_nextafter
#define nexttowardf __builtin_nexttowardf
#define nexttowardl __builtin_nexttowardl
#define nexttoward __builtin_nexttoward
#define remainderf __builtin_remainderf
#define remainderl __builtin_remainderl
#define remainder __builtin_remainder
#define remquof __builtin_remquof
#define remquol __builtin_remquol
#define remquo __builtin_remquo
#define rintf __builtin_rintf
#define rintl __builtin_rintl
#define rint __builtin_rint
#define roundf __builtin_roundf
#define roundl __builtin_roundl
#define round __builtin_round
#define scalblnf __builtin_scalblnf
#define scalblnl __builtin_scalblnl
#define scalbln __builtin_scalbln
#define scalbnf __builtin_scalbnf
#define scalbnl __builtin_scalbnl
#define scalbn __builtin_scalbn
#define snprintf __builtin_snprintf
#define tgammaf __builtin_tgammaf
#define tgammal __builtin_tgammal
#define tgamma __builtin_tgamma
#define truncf __builtin_truncf
#define truncl __builtin_truncl
#define trunc __builtin_trunc

#define acosf __builtin_acosf
#define acosl __builtin_acosl
#define asinf __builtin_asinf
#define asinl __builtin_asinl
#define atan2f __builtin_atan2f
#define atan2l __builtin_atan2l
#define atanf __builtin_atanf
#define atanl __builtin_atanl
#define ceilf __builtin_ceilf
#define ceill __builtin_ceill
#define cosf __builtin_cosf
#define coshf __builtin_coshf
#define coshl __builtin_coshl
#define cosl __builtin_cosl
#define expf __builtin_expf
#define expl __builtin_expl
#define fabsf __builtin_fabsf
#define fabsl __builtin_fabsl
#define floorf __builtin_floorf
#define floorl __builtin_floorl
#define fmodf __builtin_fmodf
#define fmodl __builtin_fmodl
#define frexpf __builtin_frexpf
#define frexpl __builtin_frexpl
#define ldexpf __builtin_ldexpf
#define ldexpl __builtin_ldexpl
#define log10f __builtin_log10f
#define log10l __builtin_log10l
#define logf __builtin_logf
#define logl __builtin_logl
#define modfl __builtin_modfl
#define modf __builtin_modf
#define modff __builtin_modff
#define powf __builtin_powf
#define powl __builtin_powl
#define sinf __builtin_sinf
#define sinhf __builtin_sinhf
#define sinhl __builtin_sinhl
#define sinl __builtin_sinl
#define sqrtf __builtin_sqrtf
#define sqrtl __builtin_sqrtl
#define tanf __builtin_tanf
#define tanhf __builtin_tanhf
#define tanhl __builtin_tanhl
#define tanl __builtin_tanl

#define acos __builtin_acos
#define asin __builtin_asin
#define atan2 __builtin_atan2
#define atan __builtin_atan
#define ceil __builtin_ceil
#define cosh __builtin_cosh
#define cos __builtin_cos
#define exp __builtin_exp
#define fabs __builtin_fabs
#define floor __builtin_floor
#define fmod __builtin_fmod
#define ldexp __builtin_ldexp
#define log10 __builtin_log10
#define log __builtin_log
#define modf __builtin_modf
#define pow __builtin_pow
#define sinh __builtin_sinh
#define sin __builtin_sin
#define sqrt __builtin_sqrt

C_HEADER_END

#endif

