/* math.h - mathematical declarations */

/*
APPLICATION USAGE

    The FP_CONTRACT pragma can be used to allow (if the state is on)
    or disallow (if the state is off) the implementation to contract
    expressions. Each pragma can occur either outside external
    declarations or preceding all explicit declarations and statements
    inside a compound statement. When outside external declarations,
    the pragma takes effect from its occurrence until another
    FP_CONTRACT pragma is encountered, or until the end of the
    translation unit. When inside a compound statement, the pragma
    takes effect from its occurrence until another FP_CONTRACT pragma
    is encountered (including within a nested compound statement),
    or until the end of the compound statement; at the end of a
    compound statement the state for the pragma is restored to its
    condition just before the compound statement. If this pragma is
    used in any other context, the behavior is undefined. The default
    state (on or off) for the pragma is implementation-defined.

    Applications should use FLT_MAX as described in the <float.h>
    header instead of the obsolescent MAXFLOAT.

RATIONALE

    Before the ISO/IEC 9899:1999 standard, the math library was
    defined only for the floating type double. All the names formed
    by appending 'f' or 'l' to a name in <math.h> were reserved
    to allow for the definition of float and long double libraries;
    and the ISO/IEC 9899:1999 standard provides for all three
    versions of math functions.

    The functions ecvt(), fcvt(), and gcvt() have been dropped
    from the ISO C standard since their capability is available
    through sprintf().
 */

#ifndef _POSIX_MATH_H
#define _POSIX_MATH_H

/*
 * The <math.h> header shall define at least the following types:
 *
 * float_t
 * A real-floating type at least as wide as float.
 *
 * double_t
 * A real-floating type at least as wide as double, and at least
 * as wide as float_t.
 *
 * If FLT_EVAL_METHOD equals 0, float_t and double_t shall be float
 * and double, respectively; if FLT_EVAL_METHOD equals 1, they shall
 * both be double; if FLT_EVAL_METHOD equals 2, they shall both be
 * long double; for other values of FLT_EVAL_METHOD, they are
 * otherwise implementation-defined.
 */

#if (FLT_EVAL_METHOD == 0)
typedef float float_t;
typedef double double_t;
#elif (FLT_EVAL_METHOD == 1)
typedef double float_t;
typedef double double_t;
#elif (FLT_EVAL_METHOD == 2)
typedef long double float_t;
typedef long double double_t;
#else /* FLT_EVAL_METHOD == others */
typedef float float_t;
typedef double double_t;
#endif /* FLT_EVAL_METHOD */

/*
 * The <math.h> header shall define the following symbolic constants.
 * The values shall have type double and shall be accurate within
 * the precision of the double type.
 */

#define M_E				(double)2.7182818284590452354	/* e */
#define M_LOG2E			(double)1.4426950408889634074	/* log 2e */
#define M_LOG10E		(double)0.43429448190325182765	/* log 10e */
#define M_LN2			(double)0.69314718055994530942	/* log e2 */
#define M_LN10			(double)2.30258509299404568402	/* log e10 */
#define M_PI			(double)3.14159265358979323846	/* pi */
#define M_PI_2			(double)1.57079632679489661923	/* pi/2 */
#define M_PI_4			(double)0.78539816339744830962	/* pi/4 */
#define M_1_PI			(double)0.31830988618379067154	/* 1/pi */
#define M_2_PI			(double)0.63661977236758134308	/* 2/pi */
#define M_2_SQRTPI		(double)1.12837916709551257390	/* 2/sqrt(pi) */
#define M_SQRT2			(double)1.41421356237309504880	/* sqrt(2) */
#define M_SQRT1_2		(double)0.70710678118654752440	/* 1/sqrt(2) */

/*
 * The <math.h> header shall define the following symbolic constant:
 *
 * MAXFLOAT - Same value as FLT_MAX in <float.h>.
 */

#define MAXFLOAT  FLT_MAX

/* Platform independent IEEE floating point special values */

/*
 * HUGE_VAL
 *
 * A positive double constant expression, not necessarily representable
 * as a float. Used as an error value returned by the mathematics library.
 * HUGE_VAL evaluates to +infinity on systems supporting IEEE Std 754-1985.
 */

#define	__HUGE_VAL_v	0x7ff0000000000000LL
#define __huge_val_t	union { unsigned char __c[8]; long long __ll; double __d; }
#define HUGE_VAL		(((__huge_val_t) { __ll: __HUGE_VAL_v }).__d)

/*
 * HUGE_VALF
 *
 * A positive float constant expression. Used as an error value returned
 * by the mathematics library. HUGE_VALF evaluates to +infinity on
 * systems supporting IEEE Std 754-1985.
 */

#define __HUGE_VALF_v	0x7f800000L
#define __huge_valf_t	union { unsigned char __c[4]; long __l; float __f; }
#define HUGE_VALF		(((__huge_valf_t) { __l: __HUGE_VALF_v }).__f)

/*
 * HUGE_VALL
 *
 * A positive long double constant expression. Used as an error value
 * returned by the mathematics library. HUGE_VALL evaluates to +infinity
 * on systems supporting IEEE Std 754-1985.
 */

#define	__HUGE_VALD_v	0x7fff000000000000UL
#define __huge_vald_t	union { struct { unsigned long __h, __l; } __i; long double __d; }}
#define HUGE_VALL		(((__huge_vald_t) { __i: {__h: __HUGE_VALD_v, __l: 0}}).__d)

/*
 * INFINITY
 *
 * A constant expression of type float representing positive or unsigned
 * infinity, if available; else a positive constant of type float that
 * overflows at translation time.
 */

#define INFINITY		HUGE_VALF

/*
 * NAN
 *
 * A constant expression of type float representing a quiet NaN.
 * This macro is only defined if the implementation supports quiet
 * NaNs for the float type.
 */

#define __NAN_VALF_v	0x7fc00000L
#define NAN				(((__huge_valf_t) { __l: __NAN_VALF_v }).__f)

/*
 * Floating-point categories
 *
 * The following macros shall be defined for number classification.
 * They represent the mutually-exclusive kinds of floating-point values.
 * They expand to integer constant expressions with distinct values.
 * Additional implementation-defined floating-point classifications,
 * with macro definitions beginning with FP_ and an uppercase letter,
 * may also be specified by the implementation.
 */

#define FP_NAN			0
#define FP_INFINITE		1
#define FP_ZERO			2
#define FP_SUBNORMAL	3
#define FP_NORMAL		4

/*
 * The following optional macros indicate whether the fma() family
 * of functions are fast compared with direct code.
 *
 * If defined, the FP_FAST_FMA macro shall expand to the integer
 * constant 1 and shall indicate that the fma() function generally
 * executes about as fast as, or faster than, a multiply and an
 * add of double operands. If undefined, the speed of execution
 * is unspecified. The other macros have the equivalent meaning
 * for the float and long double versions.
 */

#define FP_FAST_FMA     1
#define FP_FAST_FMAF    FP_FAST_FMA
#define FP_FAST_FMAL    FP_FAST_FMA

/*
 * The following macros shall expand to integer constant expressions
 * whose values are returned by ilogb(x) if x is zero or NaN,
 * respectively. The value of FP_ILOGB0 shall be either {INT_MIN} or
 * - {INT_MAX}. The value of FP_ILOGBNAN shall be either {INT_MAX} or
 * {INT_MIN}.
 */

#define FP_ILOGB0   (-INT_MAX)
#define FP_ILOGBNAN (INT_MAX)

/*
 * The following macros shall expand to the integer constants 1 and 2,
 * respectively;
 */

#define MATH_ERRNO          1
#define MATH_ERREXCEPT      2

/*
 * The following macro shall expand to an expression that has type int
 * and the value MATH_ERRNO, MATH_ERREXCEPT, or the bitwise-inclusive
 * OR of both:
 *
 * math_errhandling
 *
 * The value of math_errhandling is constant for the duration of the
 * program. It is unspecified whether math_errhandling is a macro or
 * an identifier with external linkage. If a macro definition is
 * suppressed or a program defines an identifier with the name
 * math_errhandling, the behavior is undefined. If the expression
 * (math_errhandling & MATH_ERREXCEPT) can be non-zero, the
 * implementation shall define the macros FE_DIVBYZERO, FE_INVALID,
 * and FE_OVERFLOW in <fenv.h>.
 */

#define math_errhandling        MATH_ERREXCEPT


#ifdef __cplusplus
struct __exception;
extern "C" int matherr(struct __exception *);
struct __exception
    {
#else
struct exception;
extern int matherr(struct exception *);
struct exception
    {
#endif
	int		type;
	char	*name;
	double	arg1;
	double	arg2;
	double	retval;
    };

#define DOMAIN		1
#define SING		2
#define OVERFLOW	3
#define UNDERFLOW	4
#define TLOSS		5
#define PLOSS		6

/* The following external variable shall be defined: */
extern int signgam;

#ifdef __cplusplus
extern "C" {
#endif

double      acos(double);
float       acosf(float);
double      acosh(double);
float       acoshf(float);
long double acoshl(long double);
long double acosl(long double);
double      asin(double);
float       asinf(float);
double      asinh(double);
float       asinhf(float);
long double asinhl(long double);
long double asinl(long double);
double      atan(double);
double      atan2(double, double);
float       atan2f(float, float);
long double atan2l(long double, long double);
float       atanf(float);
double      atanh(double);
float       atanhf(float);
long double atanhl(long double);
long double atanl(long double);
double      cbrt(double);
float       cbrtf(float);
long double cbrtl(long double);
double      ceil(double);
float       ceilf(float);
long double ceill(long double);
double      copysign(double, double);
float       copysignf(float, float);
long double copysignl(long double, long double);
double      cos(double);
float       cosf(float);
double      cosh(double);
float       coshf(float);
long double coshl(long double);
long double cosl(long double);
double      erf(double);
double      erfc(double);
float       erfcf(float);
long double erfcl(long double);
float       erff(float);
long double erfl(long double);
double      exp(double);
double      exp2(double);
float       exp2f(float);
long double exp2l(long double);
float       expf(float);
long double expl(long double);
double      expm1(double);
float       expm1f(float);
long double expm1l(long double);
double      fabs(double);
float       fabsf(float);
long double fabsl(long double);
double      fdim(double, double);
float       fdimf(float, float);
long double fdiml(long double, long double);
double      floor(double);
float       floorf(float);
long double floorl(long double);
double      fma(double, double, double);
float       fmaf(float, float, float);
long double fmal(long double, long double, long double);
double      fmax(double, double);
float       fmaxf(float, float);
long double fmaxl(long double, long double);
double      fmin(double, double);
float       fminf(float, float);
long double fminl(long double, long double);
double      fmod(double, double);
float       fmodf(float, float);
long double fmodl(long double, long double);
double      frexp(double, int *);
float       frexpf(float, int *);
long double frexpl(long double, int *);
double      hypot(double, double);
float       hypotf(float, float);
long double hypotl(long double, long double);
int         ilogb(double);
int         ilogbf(float);
int         ilogbl(long double);
double      j0(double);
double      j1(double);
double      jn(int, double);

double      ldexp(double, int);
float       ldexpf(float, int);
long double ldexpl(long double, int);
double      lgamma(double);
float       lgammaf(float);
long double lgammal(long double);
long long   llrint(double);
long long   llrintf(float);
long long   llrintl(long double);
long long   llround(double);
long long   llroundf(float);
long long   llroundl(long double);
double      log(double);
double      log10(double);
float       log10f(float);
long double log10l(long double);
double      log1p(double);
float       log1pf(float);
long double log1pl(long double);
double      log2(double);
float       log2f(float);
long double log2l(long double);
double      logb(double);
float       logbf(float);
long double logbl(long double);
float       logf(float);
long double logl(long double);
long        lrint(double);
long        lrintf(float);
long        lrintl(long double);
long        lround(double);
long        lroundf(float);
long        lroundl(long double);
double      modf(double, double *);
float       modff(float, float *);
long double modfl(long double, long double *);
double      nan(const char *);
float       nanf(const char *);
long double nanl(const char *);
double      nearbyint(double);
float       nearbyintf(float);
long double nearbyintl(long double);
double      nextafter(double, double);
float       nextafterf(float, float);
long double nextafterl(long double, long double);
double      nexttoward(double, long double);
float       nexttowardf(float, long double);
long double nexttowardl(long double, long double);
double      pow(double, double);
float       powf(float, float);
long double powl(long double, long double);
double      remainder(double, double);
float       remainderf(float, float);
long double remainderl(long double, long double);
double      remquo(double, double, int *);
float       remquof(float, float, int *);
long double remquol(long double, long double, int *);
double      rint(double);
float       rintf(float);
long double rintl(long double);
double      round(double);
float       roundf(float);
long double roundl(long double);
double      scalbln(double, long);
float       scalblnf(float, long);
long double scalblnl(long double, long);
double      scalbn(double, int);
float       scalbnf(float, int);
long double scalbnl(long double, int);
double      sin(double);
float       sinf(float);
double      sinh(double);
float       sinhf(float);
long double sinhl(long double);
long double sinl(long double);
double      sqrt(double);
float       sqrtf(float);
long double sqrtl(long double);
double      tan(double);
float       tanf(float);
double      tanh(double);
float       tanhf(float);
long double tanhl(long double);
long double tanl(long double);
double      tgamma(double);
float       tgammaf(float);
long double tgammal(long double);
double      trunc(double);
float       truncf(float);
long double truncl(long double);
double      y0(double);
double      y1(double);
double      yn(int, double);

/* prototypes for functions used in the macros below */
extern int			__fpclassifyf(float value);
extern int			__signbitf(float value);
extern int			__finitef(float value);
extern int			__isnanf(float value);
extern int			__isinff(float value);

extern int			__fpclassifyl(long double value);
extern int			__signbitl(long double value);
extern int			__finitel(long double value);
extern int			__isnanl(long double value);
extern int			__isinfl(long double value);

extern int			__fpclassify(double value);
extern int			__signbit(double value);
extern int			__finite(double value);
extern int			__isnan(double value);
extern int			__isinf(double value);

/*
 * The <math.h> header shall define the following macros,
 * where real-floating indicates that the argument shall
 * be an expression of real-floating type.
 */

#ifdef __MATH_BUILTIN_RELOPS
#define isgreater(x, y)         __builtin_isgreater((x), (y))
#define isgreaterequal(x, y)    __builtin_isgreaterequal((x), (y))
#define isless(x, y)            __builtin_isless((x), (y))
#define islessequal(x, y)       __builtin_islessequal((x), (y))
#define islessgreater(x, y)     __builtin_islessgreater((x), (y))
#define isunordered(x, y)       __builtin_isunordered((x), (y))
#else
#define isgreater(x, y)         (!isunordered((x), (y)) && (x) > (y))
#define isgreaterequal(x, y)    (!isunordered((x), (y)) && (x) >= (y))
#define isless(x, y)            (!isunordered((x), (y)) && (x) < (y))
#define islessequal(x, y)       (!isunordered((x), (y)) && (x) <= (y))
#define islessgreater(x, y)     (!isunordered((x), (y)) && \
                                         ((x) > (y) || (y) > (x)))
#define isunordered(x, y)       (isnan(x) || isnan(y))
#endif /* __MATH_BUILTIN_RELOPS */

/*
 * int fpclassify(real-floating x);
 *
 * returns number of classification appropriate for 'value'
 */

#define fpclassify(value) \
	(sizeof(value) == sizeof(float) ? __fpclassifyf(value)		\
		: sizeof(value) == sizeof(double) ? __fpclassify(value)	\
		: __fpclassifyl(value))

/*
 * int signbit(real-floating x);
 *
 * returns non-zero if 'value' is negative
 */

#define signbit(value) \
	(sizeof(value) == sizeof(float) ? __signbitf(value)			\
		: sizeof(value) == sizeof(double) ? __signbit(value)	\
		: __signbitl(value))

/*
 * int isfinite(real-floating x);
 *
 * returns non-zero if 'value' is not Inf or NaN
 */

#define isfinite(value) \
	(sizeof(value) == sizeof(float) ? __finitef(value)			\
		: sizeof(value) == sizeof(double) ? __finite(value)		\
		: __finitel(value))

/*
 * int isnormal(real-floating x);
 *
 * returns non-zero if 'value' is neither zero, sub-normal,
 * Inf, nor NaN
 */

#define isnormal(value) \
	(fpclassify(value) == FP_NORMAL)

/*
 * int isnan(real-floating x);
 *
 * returns non-zero if 'value' is NaN
 */

#define isnan(value) \
	(sizeof(value) == sizeof(float) ? __isnanf(value)			\
		: sizeof(value) == sizeof(double) ? __isnan(value)		\
		: __isnanl(value))

/*
 * int isinf(real-floating x);
 *
 * returns non-zero if 'value' is Inf
 */

#define isinf(value) \
	(sizeof(value) == sizeof(float) ? __isinff(value)			\
		: sizeof(value) == sizeof(double) ? __isinf(value)		\
		: __isinfl(value))

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_MATH_H */
