/* 
	Editor: http://www.visualmicro.com
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino/Genuino Zero (Native USB Port), Platform=samd, Package=arduino
*/

#if defined(_VMICRO_INTELLISENSE)

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define F_CPU 48000000L
#define ARDUINO 10805
#define ARDUINO_SAMD_ZERO
#define ARDUINO_ARCH_SAMD
#define __SAMD21G18A__
#define USB_VID 0x2341
#define USB_PID 0x804d
#define USBCON
#define __cplusplus 201103L
#define __ARM__
#define __arm__
#define __inline__
#define __asm__(x)
#define __attribute__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __volatile__
typedef int __SIZE_TYPE__;
typedef int __builtin_va_list;
#define _Pragma(x)
#define __ASM
#define __INLINE
#define __GNUC__ 0
#define __ICCARM__
#define __ARMCC_VERSION 400678
#define __cplusplus 201103L

//#define __attribute__(noinline)

#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}


#include <SAMD21G18a.h>


#include <arduino.h>
#include <pins_arduino.h> 
#include <variant.h> 
#include <variant.cpp> 
#ifndef __math_68881
extern double atan (double);
extern double cos (double);
extern double sin (double);
extern double tan (double);
extern double tanh (double);
extern double frexp (double, int *);
extern double modf (double, double *);
extern double ceil (double);
extern double fabs (double);
extern double floor (double);
#endif /* ! defined (__math_68881) */

/* Non reentrant ANSI C functions.  */

#ifndef _REENT_ONLY
#ifndef __math_68881
extern double acos (double);
extern double asin (double);
extern double atan2 (double, double);
extern double cosh (double);
extern double sinh (double);
extern double exp (double);
extern double ldexp (double, int);
extern double log (double);
extern double log10 (double);
extern double pow (double, double);
extern double sqrt (double);
extern double fmod (double, double);
#endif /* ! defined (__math_68881) */
#endif /* ! defined (_REENT_ONLY) */

extern int __isinff (float x);
extern int __isinfd (double x);
extern int __isnanf (float x);
extern int __isnand (double x);
extern int __fpclassifyf (float x);
extern int __fpclassifyd (double x);
extern int __signbitf (float x);
extern int __signbitd (double x);

//#ifndef isnan
//  #define isnan(y) (fpclassify(y) == FP_NAN)
//#endif


/* Non ANSI long double precision functions.  */

extern int finitel (long double);

/* Non ANSI double precision functions.  */

extern double infinity (void);
extern double nan (const char *);
extern int finite (double);
extern double copysign (double, double);
extern double logb (double);
extern int ilogb (double);

extern double asinh (double);
extern double cbrt (double);
extern double nextafter (double, double);
extern double rint (double);
extern double scalbn (double, int);

extern double exp2 (double);
extern double scalbln (double, long int);
extern double tgamma (double);
extern double nearbyint (double);
extern long int lrint (double);
extern long long int llrint (double);
extern double round (double);
extern long int lround (double);
extern long long int llround (double);
extern double trunc (double);
extern double remquo (double, double, int *);
extern double fdim (double, double);
extern double fmax (double, double);
extern double fmin (double, double);
extern double fma (double, double, double);

#ifndef __math_68881
extern double log1p (double);
extern double expm1 (double);
#endif /* ! defined (__math_68881) */

#ifndef _REENT_ONLY
extern double acosh (double);
extern double atanh (double);
extern double remainder (double, double);
extern double gamma (double);
extern double lgamma (double);
extern double erf (double);
extern double erfc (double);
extern double log2 (double);
#if !defined(__cplusplus)
#define log2(x) (log (x) / _M_LN2)
#endif

#ifndef __math_68881
extern double hypot (double, double);
#endif

#endif /* ! defined (_REENT_ONLY) */

/* Single precision versions of ANSI functions.  */

extern float atanf (float);
extern float cosf (float);
extern float sinf (float);
extern float tanf (float);
extern float tanhf (float);
extern float frexpf (float, int *);
extern float modff (float, float *);
extern float ceilf (float);
extern float fabsf (float);
extern float floorf (float);

#ifndef _REENT_ONLY
extern float acosf (float);
extern float asinf (float);
extern float atan2f (float, float);
extern float coshf (float);
extern float sinhf (float);
extern float expf (float);
extern float ldexpf (float, int);
extern float logf (float);
extern float log10f (float);
extern float powf (float, float);
extern float sqrtf (float);
extern float fmodf (float, float);
#endif /* ! defined (_REENT_ONLY) */

/* Other single precision functions.  */

extern float exp2f (float);
extern float scalblnf (float, int);
extern float tgammaf (float);
extern float nearbyintf (float);
extern long int lrintf (float);
extern long long int llrintf (float);
extern float roundf (float);
extern long int lroundf (float);
extern long long int llroundf (float);
extern float truncf (float);
extern float remquof (float, float, int *);
extern float fdimf (float, float);
extern float fmaxf (float, float);
extern float fminf (float, float);
extern float fmaf (float, float, float);

extern float infinityf (void);
extern float nanf (const char *);
extern int finitef (float);
extern float copysignf (float, float);
extern float logbf (float);
extern int ilogbf (float);

extern float asinhf (float);
extern float cbrtf (float);
extern float nextafterf (float, float);
extern float rintf (float);
extern float scalbnf (float, int);
extern float log1pf (float);
extern float expm1f (float);

#ifndef _REENT_ONLY
extern float acoshf (float);
extern float atanhf (float);
extern float remainderf (float, float);
extern float gammaf (float);
extern float lgammaf (float);
extern float erff (float);
extern float erfcf (float);
extern float log2f (float);
extern float hypotf (float, float);
#endif /* ! defined (_REENT_ONLY) */

/* On platforms where long double equals double.  */
#ifdef _LDBL_EQ_DBL
/* Reentrant ANSI C functions.  */
#ifndef __math_68881
extern long double atanl (long double);
extern long double cosl (long double);
extern long double sinl (long double);
extern long double tanl (long double);
extern long double tanhl (long double);
extern long double frexpl (long double, int *);
extern long double modfl (long double, long double *);
extern long double ceill (long double);
extern long double fabsl (long double);
extern long double floorl (long double);
extern long double log1pl (long double);
extern long double expm1l (long double);
#endif /* ! defined (__math_68881) */
/* Non reentrant ANSI C functions.  */
#ifndef _REENT_ONLY
#ifndef __math_68881
extern long double acosl (long double);
extern long double asinl (long double);
extern long double atan2l (long double, long double);
extern long double coshl (long double);
extern long double sinhl (long double);
extern long double expl (long double);
extern long double ldexpl (long double, int);
extern long double logl (long double);
extern long double log10l (long double);
extern long double powl (long double, long double);
extern long double sqrtl (long double);
extern long double fmodl (long double, long double);
extern long double hypotl (long double, long double);
#endif /* ! defined (__math_68881) */
#endif /* ! defined (_REENT_ONLY) */
extern long double copysignl (long double, long double);
extern long double nanl (const char *);
extern int ilogbl (long double);
extern long double asinhl (long double);
extern long double cbrtl (long double);
extern long double nextafterl (long double, long double);
extern float nexttowardf (float, long double);
extern double nexttoward (double, long double);
extern long double nexttowardl (long double, long double);
extern long double logbl (long double);
extern long double log2l (long double);
extern long double rintl (long double);
extern long double scalbnl (long double, int);
extern long double exp2l (long double);
extern long double scalblnl (long double, long);
extern long double tgammal (long double);
extern long double nearbyintl (long double);
extern long int lrintl (long double);
extern long long int llrintl (long double);
extern long double roundl (long double);
extern long lroundl (long double);
extern long long int llroundl (long double);
extern long double truncl (long double);
extern long double remquol (long double, long double, int *);
extern long double fdiml (long double, long double);
extern long double fmaxl (long double, long double);
extern long double fminl (long double, long double);
extern long double fmal (long double, long double, long double);
#ifndef _REENT_ONLY
extern long double acoshl (long double);
extern long double atanhl (long double);
extern long double remainderl (long double, long double);
extern long double lgammal (long double);
extern long double erfl (long double);
extern long double erfcl (long double);
#endif /* ! defined (_REENT_ONLY) */
#else /* !_LDBL_EQ_DBL */
extern long double hypotl (long double, long double);
extern long double sqrtl (long double);
#ifdef __i386__
/* Other long double precision functions.  */
extern _LONG_DOUBLE rintl _PARAMS((_LONG_DOUBLE));
extern long int lrintl _PARAMS((_LONG_DOUBLE));
extern long long int llrintl _PARAMS((_LONG_DOUBLE));
#endif /* __i386__ */
#endif /* !_LDBL_EQ_DBL */

#endif /* !defined (__STRICT_ANSI__) || defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) */

#if !defined (__STRICT_ANSI__) || defined(__cplusplus)

extern double drem (double, double);
extern void sincos (double, double *, double *);
extern double gamma_r (double, int *);
extern double lgamma_r (double, int *);

extern double y0 (double);
extern double y1 (double);
extern double yn (int, double);
extern double j0 (double);
extern double j1 (double);
extern double jn (int, double);

extern float dremf (float, float);
extern void sincosf (float, float *, float *);
extern float gammaf_r (float, int *);
extern float lgammaf_r (float, int *);

extern float y0f (float);
extern float y1f (float);
extern float ynf (int, float);
extern float j0f (float);
extern float j1f (float);
extern float jnf (int, float);

/* GNU extensions */
# ifndef exp10
extern double exp10 (double);
# endif
# ifndef pow10
extern double pow10 (double);
# endif
# ifndef exp10f
extern float exp10f (float);
# endif
# ifndef pow10f
extern float pow10f (float);
# endif

#endif /* !defined (__STRICT_ANSI__) || defined(__cplusplus) */

#ifndef __STRICT_ANSI__

/* The gamma functions use a global variable, signgam.  */
#ifndef _REENT_ONLY
//#define signgam (*__signgam())
extern int *__signgam (void);
#endif /* ! defined (_REENT_ONLY) */

//#define __signgam_r(ptr) _REENT_SIGNGAM(ptr)


#ifdef __cplusplus
extern int matherr (struct __exception *e);
#else
extern int matherr (struct exception *e);
#endif

///* Values for the type field of struct exception.  */
//
//#define DOMAIN 1
//#define SING 2
//#define OVERFLOW 3
//#define UNDERFLOW 4
//#define TLOSS 5
//#define PLOSS 6
//
//#endif /* ! defined (__STRICT_ANSI__) */
//
///* Useful constants.  */
//
//#if !defined(__STRICT_ANSI__) || ((_XOPEN_SOURCE - 0) >= 500)
//
//#define MAXFLOAT	3.40282347e+38F
//
//#define M_E		2.7182818284590452354
//#define M_LOG2E		1.4426950408889634074
//#define M_LOG10E	0.43429448190325182765
//#define M_LN2		_M_LN2
//#define M_LN10		2.30258509299404568402
//#define M_PI		3.14159265358979323846
//#define M_PI_2		1.57079632679489661923
//#define M_PI_4		0.78539816339744830962
//#define M_1_PI		0.31830988618379067154
//#define M_2_PI		0.63661977236758134308
//#define M_2_SQRTPI	1.12837916709551257390
//#define M_SQRT2		1.41421356237309504880
//#define M_SQRT1_2	0.70710678118654752440
//
//#endif
//
//#ifndef __STRICT_ANSI__
//
//#define M_TWOPI         (M_PI * 2.0)
//#define M_3PI_4		2.3561944901923448370E0
//#define M_SQRTPI        1.77245385090551602792981
//#define M_LN2LO         1.9082149292705877000E-10
//#define M_LN2HI         6.9314718036912381649E-1
//#define M_SQRT3	1.73205080756887719000
//#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
//#define M_LOG2_E        _M_LN2
//#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */
//
///* Global control over fdlibm error handling.  */
//
//enum __fdlibm_version
//{
//  __fdlibm_ieee = -1,
//  __fdlibm_svid,
//  __fdlibm_xopen,
//  __fdlibm_posix
//};
//
//#define _LIB_VERSION_TYPE enum __fdlibm_version
//#define _LIB_VERSION __fdlib_version
//
//extern __IMPORT _LIB_VERSION_TYPE _LIB_VERSION;
//
//#define _IEEE_  __fdlibm_ieee
//#define _SVID_  __fdlibm_svid
//#define _XOPEN_ __fdlibm_xopen
//#define _POSIX_ __fdlibm_posix
//
//#endif /* ! defined (__STRICT_ANSI__) */
//
//_END_STD_C
//
//#ifdef __FAST_MATH__
//#include <machine/fastmath.h>
//#endif
//
//#endif /* _MATH_H_ */
#include "USB_Host_SAMD21_desc.ino"
#endif
#endif
