/* tgmath.h - type-generic macros */

/*
DESCRIPTION

    Of the functions contained within the <math.h> and <complex.h> headers
    without an f ( float) or l ( long double) suffix, several have one or
    more parameters whose corresponding real type is double. For each such
    function, except modf(), [XSI] [Option Start] j0(), j1(), jn(), y0(),
    y1(), and yn(), [Option End]  there shall be a corresponding type-generic
    macro. The parameters whose corresponding real type is double in the
    function synopsis are generic parameters. Use of the macro invokes a
    function whose corresponding real type and type domain are determined
    by the arguments for the generic parameters.

    Use of the macro invokes a function whose generic parameters have the
    corresponding real type determined as follows:

    - First, if any argument for generic parameters has type long double,
      the type determined is long double.

    - Otherwise, if any argument for generic parameters has type double or
      is of integer type, the type determined is double.

    - Otherwise, the type determined is float.

    For each unsuffixed function in the <math.h> header for which there is
    a function in the <complex.h> header with the same name except for a c
    prefix, the corresponding type-generic macro (for both functions) has
    the same name as the function in the <math.h> header. The corresponding
    type-generic macro for fabs() and cabs() is fabs().

    If at least one argument for a generic parameter is complex, then use
    of the macro invokes a complex function; otherwise, use of the macro
    invokes a real function.

    For each unsuffixed function in the <math.h> header without a c-prefixed
    counterpart in the <complex.h> header, except for modf(), j0(), j1(), jn(),
    y0(), y1(), and yn(), [Option End]  the corresponding type-generic macro
    has the same name as the function. These type-generic macros are:


    atan2()
    cbrt()
    ceil()
    copysign()
    erf()
    erfc()
    exp2()
    expm1()
    fdim()
    floor()


    fma()
    fmax()
    fmin()
    fmod()
    frexp()
    hypot()
    ilogb()
    ldexp()
    lgamma()
    llrint()


    llround()
    log10()
    log1p()
    log2()
    logb()
    lrint()
    lround()
    nearbyint()
    nextafter()
    nexttoward()

    remainder()
    remquo()
    rint()
    round()
    scalbn()
    scalbln()
    tgamma()
    trunc()

    If all arguments for generic parameters are real, then use of the
    macro invokes a real function; otherwise, use of the macro results
    in undefined behavior.

    For each unsuffixed function in the <complex.h> header that is not
    a c-prefixed counterpart to a function in the <math.h> header, the
    corresponding type-generic macro has the same name as the function.
    These type-generic macros are:

    carg()
    cimag()
    conj()
    cproj()
    creal()

    Use of the macro with any real or complex argument invokes a complex
    function.

APPLICATION USAGE

    With the declarations:

    #include <tgmath.h>
    int n;
    float f;
    double d;
    long double ld;
    float complex fc;
    double complex dc;
    long double complex ldc;

    functions invoked by use of type-generic macros are shown in the
    following table:

    Macro       -       Use Invokes

    exp(n)              exp(n), the function

    acosh(f)            acoshf(f)

    sin(d)              sin(d), the function

    atan(ld)            atanl(ld)

    log(fc)             clogf(fc)

    sqrt(dc)            csqrt(dc)

    pow(ldc,f)          cpowl(ldc, f)

    remainder(n,n)      remainder(n, n), the function

    nextafter(d,f)      nextafter(d, f), the function

    nexttoward(f,ld)    nexttowardf(f, ld)

    copysign(n,ld)      copysignl(n, ld)

    ceil(fc)            Undefined behavior

    rint(dc)            Undefined behavior

    fmax(ldc,ld)        Undefined behavior

    carg(n)             carg(n), the function

    cproj(f)            cprojf(f)

    creal(d)            creal(d), the function

    cimag(ld)           cimagl(ld)

    cabs(fc)            cabsf(fc)

    carg(dc)            carg(dc), the function

    cproj(ldc)          cprojl(ldc)

RATIONALE

    Type-generic macros allow calling a function whose type is determined
    by the argument type, as is the case for C operators such as '+' and '*'.
    For example, with a type-generic cos() macro, the expression cos(( float) x)
    will have type float. This feature enables writing more portably efficient
    code and alleviates need for awkward casting and suffixing in the process
    of porting or adjusting precision. Generic math functions are a widely
    appreciated feature of Fortran.

    The only arguments that affect the type resolution are the arguments
    corresponding to the parameters that have type double in the synopsis.
    Hence the type of a type-generic call to nexttoward(), whose second
    parameter is long double in the synopsis, is determined solely by the
    type of the first argument.

    The term "type-generic" was chosen over the proposed alternatives of
    intrinsic and overloading. The term is more specific than intrinsic,
    which already is widely used with a more general meaning, and reflects
    a closer match to Fortran's generic functions than to C++ overloading.

    The macros are placed in their own header in order not to silently
    break old programs that include the <math.h> header; for example, with:

    printf ("%e", sin(x))

    modf( double, double *) is excluded because no way was seen to make it
    safe without complicating the type resolution.

    The implementation might, as an extension, endow appropriate ones of the
    macros that POSIX.1-2008 specifies only for real arguments with the
    ability to invoke the complex functions.

    POSIX.1-2008 does not prescribe any particular implementation mechanism
    for generic macros. It could be implemented simply with built-in macros.
    The generic macro for sqrt(), for example, could be implemented with:

    #undef sqrt
    #define sqrt(x) __BUILTIN_GENERIC_sqrt(x)

    Generic macros are designed for a useful level of consistency with C++
    overloaded math functions.

    The great majority of existing C programs are expected to be unaffected
    when the <tgmath.h> header is included instead of the <math.h> or
    <complex.h> headers. Generic macros are similar to the ISO/IEC 9899:1999
    standard library masking macros, though the semantic types of return
    values differ.

    The ability to overload on integer as well as floating types would have
    been useful for some functions; for example, copysign(). Overloading with
    different numbers of arguments would have allowed reusing names; for
    example, remainder() for remquo(). However, these facilities would have
    complicated the specification; and their natural consistent use, such as
    for a floating abs() or a two-argument atan(), would have introduced
    further inconsistencies with the ISO/IEC 9899:1999 standard for
    insufficient benefit.

    The ISO C standard in no way limits the implementation's options for
    efficiency, including inlining library functions.
*/
#ifndef _POSIX_TGMATH_H
#define _POSIX_TGMATH_H

/*
 * The <tgmath.h> header shall include the headers <math.h> and <complex.h>
 * and shall define several type-generic macros.
 */
#include <math.h>
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _TGMATH_ATTRSp __attribute__((__overloadable__))
#define _TGMATH_ATTRS __attribute__((__overloadable__, __always_inline__))

/* promotion */

typedef void _Argument_type_is_not_arithmetic;
static _Argument_type_is_not_arithmetic __tg_promote(...)
  __attribute__((__unavailable__,__overloadable__));
static double               _TGMATH_ATTRSp __tg_promote(int);
static double               _TGMATH_ATTRSp __tg_promote(unsigned int);
static double               _TGMATH_ATTRSp __tg_promote(long);
static double               _TGMATH_ATTRSp __tg_promote(unsigned long);
static double               _TGMATH_ATTRSp __tg_promote(long long);
static double               _TGMATH_ATTRSp __tg_promote(unsigned long long);
static float                _TGMATH_ATTRSp __tg_promote(float);
static double               _TGMATH_ATTRSp __tg_promote(double);
static long double          _TGMATH_ATTRSp __tg_promote(long double);
static float _Complex       _TGMATH_ATTRSp __tg_promote(float _Complex);
static double _Complex      _TGMATH_ATTRSp __tg_promote(double _Complex);
static long double _Complex _TGMATH_ATTRSp __tg_promote(long double _Complex);

#define __tgmath_promote1(__x)           (__typeof__(__tg_promote(__x)))
#define __tgmath_promote2(__x, __y)      (__typeof__(__tg_promote(__x) + \
                                                 __tg_promote(__y)))
#define __tgmath_promote3(__x, __y, __z) (__typeof__(__tg_promote(__x) + \
                                                 __tg_promote(__y) + \
                                                 __tg_promote(__z)))

/* acos */

static float
    _TGMATH_ATTRS
    __tg_acos(float __x) {return acosf(__x);}

static double
    _TGMATH_ATTRS
    __tg_acos(double __x) {return acos(__x);}

static long double
    _TGMATH_ATTRS
    __tg_acos(long double __x) {return acosl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_acos(float _Complex __x) {return cacosf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_acos(double _Complex __x) {return cacos(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_acos(long double _Complex __x) {return cacosl(__x);}

#undef acos
#define acos(__x) __tg_acos(__tgmath_promote1((__x))(__x))

/* asin */

static float
    _TGMATH_ATTRS
    __tg_asin(float __x) {return asinf(__x);}

static double
    _TGMATH_ATTRS
    __tg_asin(double __x) {return asin(__x);}

static long double
    _TGMATH_ATTRS
    __tg_asin(long double __x) {return asinl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_asin(float _Complex __x) {return casinf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_asin(double _Complex __x) {return casin(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_asin(long double _Complex __x) {return casinl(__x);}

#undef asin
#define asin(__x) __tg_asin(__tgmath_promote1((__x))(__x))

/* atan */

static float
    _TGMATH_ATTRS
    __tg_atan(float __x) {return atanf(__x);}

static double
    _TGMATH_ATTRS
    __tg_atan(double __x) {return atan(__x);}

static long double
    _TGMATH_ATTRS
    __tg_atan(long double __x) {return atanl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_atan(float _Complex __x) {return catanf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_atan(double _Complex __x) {return catan(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_atan(long double _Complex __x) {return catanl(__x);}

#undef atan
#define atan(__x) __tg_atan(__tgmath_promote1((__x))(__x))

/* acosh */

static float
    _TGMATH_ATTRS
    __tg_acosh(float __x) {return acoshf(__x);}

static double
    _TGMATH_ATTRS
    __tg_acosh(double __x) {return acosh(__x);}

static long double
    _TGMATH_ATTRS
    __tg_acosh(long double __x) {return acoshl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_acosh(float _Complex __x) {return cacoshf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_acosh(double _Complex __x) {return cacosh(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_acosh(long double _Complex __x) {return cacoshl(__x);}

#undef acosh
#define acosh(__x) __tg_acosh(__tgmath_promote1((__x))(__x))

/* asinh */

static float
    _TGMATH_ATTRS
    __tg_asinh(float __x) {return asinhf(__x);}

static double
    _TGMATH_ATTRS
    __tg_asinh(double __x) {return asinh(__x);}

static long double
    _TGMATH_ATTRS
    __tg_asinh(long double __x) {return asinhl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_asinh(float _Complex __x) {return casinhf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_asinh(double _Complex __x) {return casinh(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_asinh(long double _Complex __x) {return casinhl(__x);}

#undef asinh
#define asinh(__x) __tg_asinh(__tgmath_promote1((__x))(__x))

/* atanh */

static float
    _TGMATH_ATTRS
    __tg_atanh(float __x) {return atanhf(__x);}

static double
    _TGMATH_ATTRS
    __tg_atanh(double __x) {return atanh(__x);}

static long double
    _TGMATH_ATTRS
    __tg_atanh(long double __x) {return atanhl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_atanh(float _Complex __x) {return catanhf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_atanh(double _Complex __x) {return catanh(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_atanh(long double _Complex __x) {return catanhl(__x);}

#undef atanh
#define atanh(__x) __tg_atanh(__tgmath_promote1((__x))(__x))

/* cos */

static float
    _TGMATH_ATTRS
    __tg_cos(float __x) {return cosf(__x);}

static double
    _TGMATH_ATTRS
    __tg_cos(double __x) {return cos(__x);}

static long double
    _TGMATH_ATTRS
    __tg_cos(long double __x) {return cosl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_cos(float _Complex __x) {return ccosf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_cos(double _Complex __x) {return ccos(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_cos(long double _Complex __x) {return ccosl(__x);}

#undef cos
#define cos(__x) __tg_cos(__tgmath_promote1((__x))(__x))

/* sin */

static float
    _TGMATH_ATTRS
    __tg_sin(float __x) {return sinf(__x);}

static double
    _TGMATH_ATTRS
    __tg_sin(double __x) {return sin(__x);}

static long double
    _TGMATH_ATTRS
    __tg_sin(long double __x) {return sinl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_sin(float _Complex __x) {return csinf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_sin(double _Complex __x) {return csin(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_sin(long double _Complex __x) {return csinl(__x);}

#undef sin
#define sin(__x) __tg_sin(__tgmath_promote1((__x))(__x))

/* tan */

static float
    _TGMATH_ATTRS
    __tg_tan(float __x) {return tanf(__x);}

static double
    _TGMATH_ATTRS
    __tg_tan(double __x) {return tan(__x);}

static long double
    _TGMATH_ATTRS
    __tg_tan(long double __x) {return tanl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_tan(float _Complex __x) {return ctanf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_tan(double _Complex __x) {return ctan(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_tan(long double _Complex __x) {return ctanl(__x);}

#undef tan
#define tan(__x) __tg_tan(__tgmath_promote1((__x))(__x))

/* cosh */

static float
    _TGMATH_ATTRS
    __tg_cosh(float __x) {return coshf(__x);}

static double
    _TGMATH_ATTRS
    __tg_cosh(double __x) {return cosh(__x);}

static long double
    _TGMATH_ATTRS
    __tg_cosh(long double __x) {return coshl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_cosh(float _Complex __x) {return ccoshf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_cosh(double _Complex __x) {return ccosh(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_cosh(long double _Complex __x) {return ccoshl(__x);}

#undef cosh
#define cosh(__x) __tg_cosh(__tgmath_promote1((__x))(__x))

/* sinh */

static float
    _TGMATH_ATTRS
    __tg_sinh(float __x) {return sinhf(__x);}

static double
    _TGMATH_ATTRS
    __tg_sinh(double __x) {return sinh(__x);}

static long double
    _TGMATH_ATTRS
    __tg_sinh(long double __x) {return sinhl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_sinh(float _Complex __x) {return csinhf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_sinh(double _Complex __x) {return csinh(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_sinh(long double _Complex __x) {return csinhl(__x);}

#undef sinh
#define sinh(__x) __tg_sinh(__tgmath_promote1((__x))(__x))

/* tanh */

static float
    _TGMATH_ATTRS
    __tg_tanh(float __x) {return tanhf(__x);}

static double
    _TGMATH_ATTRS
    __tg_tanh(double __x) {return tanh(__x);}

static long double
    _TGMATH_ATTRS
    __tg_tanh(long double __x) {return tanhl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_tanh(float _Complex __x) {return ctanhf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_tanh(double _Complex __x) {return ctanh(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_tanh(long double _Complex __x) {return ctanhl(__x);}

#undef tanh
#define tanh(__x) __tg_tanh(__tgmath_promote1((__x))(__x))

/* exp */

static float
    _TGMATH_ATTRS
    __tg_exp(float __x) {return expf(__x);}

static double
    _TGMATH_ATTRS
    __tg_exp(double __x) {return exp(__x);}

static long double
    _TGMATH_ATTRS
    __tg_exp(long double __x) {return expl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_exp(float _Complex __x) {return cexpf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_exp(double _Complex __x) {return cexp(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_exp(long double _Complex __x) {return cexpl(__x);}

#undef exp
#define exp(__x) __tg_exp(__tgmath_promote1((__x))(__x))

/* log */

static float
    _TGMATH_ATTRS
    __tg_log(float __x) {return logf(__x);}

static double
    _TGMATH_ATTRS
    __tg_log(double __x) {return log(__x);}

static long double
    _TGMATH_ATTRS
    __tg_log(long double __x) {return logl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_log(float _Complex __x) {return clogf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_log(double _Complex __x) {return clog(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_log(long double _Complex __x) {return clogl(__x);}

#undef log
#define log(__x) __tg_log(__tgmath_promote1((__x))(__x))

/* pow */

static float
    _TGMATH_ATTRS
    __tg_pow(float __x, float __y) {return powf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_pow(double __x, double __y) {return pow(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_pow(long double __x, long double __y) {return powl(__x, __y);}

static float _Complex
    _TGMATH_ATTRS
    __tg_pow(float _Complex __x, float _Complex __y) {return cpowf(__x, __y);}

static double _Complex
    _TGMATH_ATTRS
    __tg_pow(double _Complex __x, double _Complex __y) {return cpow(__x, __y);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_pow(long double _Complex __x, long double _Complex __y)
    {return cpowl(__x, __y);}

#undef pow
#define pow(__x, __y) __tg_pow(__tgmath_promote2((__x), (__y))(__x), \
                               __tgmath_promote2((__x), (__y))(__y))

/* sqrt */

static float
    _TGMATH_ATTRS
    __tg_sqrt(float __x) {return sqrtf(__x);}

static double
    _TGMATH_ATTRS
    __tg_sqrt(double __x) {return sqrt(__x);}

static long double
    _TGMATH_ATTRS
    __tg_sqrt(long double __x) {return sqrtl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_sqrt(float _Complex __x) {return csqrtf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_sqrt(double _Complex __x) {return csqrt(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_sqrt(long double _Complex __x) {return csqrtl(__x);}

#undef sqrt
#define sqrt(__x) __tg_sqrt(__tgmath_promote1((__x))(__x))

/* fabs */

static float
    _TGMATH_ATTRS
    __tg_fabs(float __x) {return fabsf(__x);}

static double
    _TGMATH_ATTRS
    __tg_fabs(double __x) {return fabs(__x);}

static long double
    _TGMATH_ATTRS
    __tg_fabs(long double __x) {return fabsl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_fabs(float _Complex __x) {return cabsf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_fabs(double _Complex __x) {return cabs(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_fabs(long double _Complex __x) {return cabsl(__x);}

#undef fabs
#define fabs(__x) __tg_fabs(__tgmath_promote1((__x))(__x))

/* atan2 */

static float
    _TGMATH_ATTRS
    __tg_atan2(float __x, float __y) {return atan2f(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_atan2(double __x, double __y) {return atan2(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_atan2(long double __x, long double __y) {return atan2l(__x, __y);}

#undef atan2
#define atan2(__x, __y) __tg_atan2(__tgmath_promote2((__x), (__y))(__x), \
                                   __tgmath_promote2((__x), (__y))(__y))

/* cbrt */

static float
    _TGMATH_ATTRS
    __tg_cbrt(float __x) {return cbrtf(__x);}

static double
    _TGMATH_ATTRS
    __tg_cbrt(double __x) {return cbrt(__x);}

static long double
    _TGMATH_ATTRS
    __tg_cbrt(long double __x) {return cbrtl(__x);}

#undef cbrt
#define cbrt(__x) __tg_cbrt(__tgmath_promote1((__x))(__x))

/* ceil */

static float
    _TGMATH_ATTRS
    __tg_ceil(float __x) {return ceilf(__x);}

static double
    _TGMATH_ATTRS
    __tg_ceil(double __x) {return ceil(__x);}

static long double
    _TGMATH_ATTRS
    __tg_ceil(long double __x) {return ceill(__x);}

#undef ceil
#define ceil(__x) __tg_ceil(__tgmath_promote1((__x))(__x))

/* copysign */

static float
    _TGMATH_ATTRS
    __tg_copysign(float __x, float __y) {return copysignf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_copysign(double __x, double __y) {return copysign(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_copysign(long double __x, long double __y) {return copysignl(__x, __y);}

#undef copysign
#define copysign(__x, __y) __tg_copysign(__tgmath_promote2((__x), (__y))(__x), \
                                         __tgmath_promote2((__x), (__y))(__y))

/* erf */

static float
    _TGMATH_ATTRS
    __tg_erf(float __x) {return erff(__x);}

static double
    _TGMATH_ATTRS
    __tg_erf(double __x) {return erf(__x);}

static long double
    _TGMATH_ATTRS
    __tg_erf(long double __x) {return erfl(__x);}

#undef erf
#define erf(__x) __tg_erf(__tgmath_promote1((__x))(__x))

/* erfc */

static float
    _TGMATH_ATTRS
    __tg_erfc(float __x) {return erfcf(__x);}

static double
    _TGMATH_ATTRS
    __tg_erfc(double __x) {return erfc(__x);}

static long double
    _TGMATH_ATTRS
    __tg_erfc(long double __x) {return erfcl(__x);}

#undef erfc
#define erfc(__x) __tg_erfc(__tgmath_promote1((__x))(__x))

/* exp2 */

static float
    _TGMATH_ATTRS
    __tg_exp2(float __x) {return exp2f(__x);}

static double
    _TGMATH_ATTRS
    __tg_exp2(double __x) {return exp2(__x);}

static long double
    _TGMATH_ATTRS
    __tg_exp2(long double __x) {return exp2l(__x);}

#undef exp2
#define exp2(__x) __tg_exp2(__tgmath_promote1((__x))(__x))

/* expm1 */

static float
    _TGMATH_ATTRS
    __tg_expm1(float __x) {return expm1f(__x);}

static double
    _TGMATH_ATTRS
    __tg_expm1(double __x) {return expm1(__x);}

static long double
    _TGMATH_ATTRS
    __tg_expm1(long double __x) {return expm1l(__x);}

#undef expm1
#define expm1(__x) __tg_expm1(__tgmath_promote1((__x))(__x))

/* fdim */

static float
    _TGMATH_ATTRS
    __tg_fdim(float __x, float __y) {return fdimf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_fdim(double __x, double __y) {return fdim(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_fdim(long double __x, long double __y) {return fdiml(__x, __y);}

#undef fdim
#define fdim(__x, __y) __tg_fdim(__tgmath_promote2((__x), (__y))(__x), \
                                 __tgmath_promote2((__x), (__y))(__y))

/* floor */

static float
    _TGMATH_ATTRS
    __tg_floor(float __x) {return floorf(__x);}

static double
    _TGMATH_ATTRS
    __tg_floor(double __x) {return floor(__x);}

static long double
    _TGMATH_ATTRS
    __tg_floor(long double __x) {return floorl(__x);}

#undef floor
#define floor(__x) __tg_floor(__tgmath_promote1((__x))(__x))

/* fma */

static float
    _TGMATH_ATTRS
    __tg_fma(float __x, float __y, float __z)
    {return fmaf(__x, __y, __z);}

static double
    _TGMATH_ATTRS
    __tg_fma(double __x, double __y, double __z)
    {return fma(__x, __y, __z);}

static long double
    _TGMATH_ATTRS
    __tg_fma(long double __x,long double __y, long double __z)
    {return fmal(__x, __y, __z);}

#undef fma
#define fma(__x, __y, __z)                                \
        __tg_fma(__tgmath_promote3((__x), (__y), (__z))(__x), \
                 __tgmath_promote3((__x), (__y), (__z))(__y), \
                 __tgmath_promote3((__x), (__y), (__z))(__z))

/* fmax */

static float
    _TGMATH_ATTRS
    __tg_fmax(float __x, float __y) {return fmaxf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_fmax(double __x, double __y) {return fmax(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_fmax(long double __x, long double __y) {return fmaxl(__x, __y);}

#undef fmax
#define fmax(__x, __y) __tg_fmax(__tgmath_promote2((__x), (__y))(__x), \
                                 __tgmath_promote2((__x), (__y))(__y))

/* fmin */

static float
    _TGMATH_ATTRS
    __tg_fmin(float __x, float __y) {return fminf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_fmin(double __x, double __y) {return fmin(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_fmin(long double __x, long double __y) {return fminl(__x, __y);}

#undef fmin
#define fmin(__x, __y) __tg_fmin(__tgmath_promote2((__x), (__y))(__x), \
                                 __tgmath_promote2((__x), (__y))(__y))

/* fmod */

static float
    _TGMATH_ATTRS
    __tg_fmod(float __x, float __y) {return fmodf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_fmod(double __x, double __y) {return fmod(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_fmod(long double __x, long double __y) {return fmodl(__x, __y);}

#undef fmod
#define fmod(__x, __y) __tg_fmod(__tgmath_promote2((__x), (__y))(__x), \
                                 __tgmath_promote2((__x), (__y))(__y))

/* frexp */

static float
    _TGMATH_ATTRS
    __tg_frexp(float __x, int* __y) {return frexpf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_frexp(double __x, int* __y) {return frexp(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_frexp(long double __x, int* __y) {return frexpl(__x, __y);}

#undef frexp
#define frexp(__x, __y) __tg_frexp(__tgmath_promote1((__x))(__x), __y)

/* hypot */

static float
    _TGMATH_ATTRS
    __tg_hypot(float __x, float __y) {return hypotf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_hypot(double __x, double __y) {return hypot(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_hypot(long double __x, long double __y) {return hypotl(__x, __y);}

#undef hypot
#define hypot(__x, __y) __tg_hypot(__tgmath_promote2((__x), (__y))(__x), \
                                   __tgmath_promote2((__x), (__y))(__y))

/* ilogb */

static int
    _TGMATH_ATTRS
    __tg_ilogb(float __x) {return ilogbf(__x);}

static int
    _TGMATH_ATTRS
    __tg_ilogb(double __x) {return ilogb(__x);}

static int
    _TGMATH_ATTRS
    __tg_ilogb(long double __x) {return ilogbl(__x);}

#undef ilogb
#define ilogb(__x) __tg_ilogb(__tgmath_promote1((__x))(__x))

/* ldexp */

static float
    _TGMATH_ATTRS
    __tg_ldexp(float __x, int __y) {return ldexpf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_ldexp(double __x, int __y) {return ldexp(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_ldexp(long double __x, int __y) {return ldexpl(__x, __y);}

#undef ldexp
#define ldexp(__x, __y) __tg_ldexp(__tgmath_promote1((__x))(__x), __y)

/* lgamma */

static float
    _TGMATH_ATTRS
    __tg_lgamma(float __x) {return lgammaf(__x);}

static double
    _TGMATH_ATTRS
    __tg_lgamma(double __x) {return lgamma(__x);}

static long double
    _TGMATH_ATTRS
    __tg_lgamma(long double __x) {return lgammal(__x);}

#undef lgamma
#define lgamma(__x) __tg_lgamma(__tgmath_promote1((__x))(__x))

/* llrint */

static long long
    _TGMATH_ATTRS
    __tg_llrint(float __x) {return llrintf(__x);}

static long long
    _TGMATH_ATTRS
    __tg_llrint(double __x) {return llrint(__x);}

static long long
    _TGMATH_ATTRS
    __tg_llrint(long double __x) {return llrintl(__x);}

#undef llrint
#define llrint(__x) __tg_llrint(__tgmath_promote1((__x))(__x))

/* llround */

static long long
    _TGMATH_ATTRS
    __tg_llround(float __x) {return llroundf(__x);}

static long long
    _TGMATH_ATTRS
    __tg_llround(double __x) {return llround(__x);}

static long long
    _TGMATH_ATTRS
    __tg_llround(long double __x) {return llroundl(__x);}

#undef llround
#define llround(__x) __tg_llround(__tgmath_promote1((__x))(__x))

/* log10 */

static float
    _TGMATH_ATTRS
    __tg_log10(float __x) {return log10f(__x);}

static double
    _TGMATH_ATTRS
    __tg_log10(double __x) {return log10(__x);}

static long double
    _TGMATH_ATTRS
    __tg_log10(long double __x) {return log10l(__x);}

#undef log10
#define log10(__x) __tg_log10(__tgmath_promote1((__x))(__x))

/* log1p */

static float
    _TGMATH_ATTRS
    __tg_log1p(float __x) {return log1pf(__x);}

static double
    _TGMATH_ATTRS
    __tg_log1p(double __x) {return log1p(__x);}

static long double
    _TGMATH_ATTRS
    __tg_log1p(long double __x) {return log1pl(__x);}

#undef log1p
#define log1p(__x) __tg_log1p(__tgmath_promote1((__x))(__x))

/* log2 */

static float
    _TGMATH_ATTRS
    __tg_log2(float __x) {return log2f(__x);}

static double
    _TGMATH_ATTRS
    __tg_log2(double __x) {return log2(__x);}

static long double
    _TGMATH_ATTRS
    __tg_log2(long double __x) {return log2l(__x);}

#undef log2
#define log2(__x) __tg_log2(__tgmath_promote1((__x))(__x))

/* lrint */

static long
    _TGMATH_ATTRS
    __tg_lrint(float __x) {return lrintf(__x);}

static long
    _TGMATH_ATTRS
    __tg_lrint(double __x) {return lrint(__x);}

static long
    _TGMATH_ATTRS
    __tg_lrint(long double __x) {return lrintl(__x);}

#undef lrint
#define lrint(__x) __tg_lrint(__tgmath_promote1((__x))(__x))

/* lround */

static long
    _TGMATH_ATTRS
    __tg_lround(float __x) {return lroundf(__x);}

static long
    _TGMATH_ATTRS
    __tg_lround(double __x) {return lround(__x);}

static long
    _TGMATH_ATTRS
    __tg_lround(long double __x) {return lroundl(__x);}

#undef lround
#define lround(__x) __tg_lround(__tgmath_promote1((__x))(__x))

/* nearbyint */

static float
    _TGMATH_ATTRS
    __tg_nearbyint(float __x) {return nearbyintf(__x);}

static double
    _TGMATH_ATTRS
    __tg_nearbyint(double __x) {return nearbyint(__x);}

static long double
    _TGMATH_ATTRS
    __tg_nearbyint(long double __x) {return nearbyintl(__x);}

#undef nearbyint
#define nearbyint(__x) __tg_nearbyint(__tgmath_promote1((__x))(__x))

/* nextafter */

static float
    _TGMATH_ATTRS
    __tg_nextafter(float __x, float __y) {return nextafterf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_nextafter(double __x, double __y) {return nextafter(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_nextafter(long double __x, long double __y) {return nextafterl(__x, __y);}

#undef nextafter
#define nextafter(__x, __y) __tg_nextafter(__tgmath_promote2((__x), (__y))(__x), \
                                           __tgmath_promote2((__x), (__y))(__y))

/* nexttoward */

static float
    _TGMATH_ATTRS
    __tg_nexttoward(float __x, float __y) {return nexttowardf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_nexttoward(double __x, double __y) {return nexttoward(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_nexttoward(long double __x, long double __y) {return nexttowardl(__x, __y);}

#undef nexttoward
#define nexttoward(__x, __y) __tg_nexttoward(__tgmath_promote2((__x), (__y))(__x), \
                                             __tgmath_promote2((__x), (__y))(__y))

/* remainder */

static float
    _TGMATH_ATTRS
    __tg_remainder(float __x, float __y) {return remainderf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_remainder(double __x, double __y) {return remainder(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_remainder(long double __x, long double __y) {return remainderl(__x, __y);}

#undef remainder
#define remainder(__x, __y) __tg_remainder(__tgmath_promote2((__x), (__y))(__x), \
                                           __tgmath_promote2((__x), (__y))(__y))

/* remquo */

static float
    _TGMATH_ATTRS
    __tg_remquo(float __x, float __y, int* __z)
    {return remquof(__x, __y, __z);}

static double
    _TGMATH_ATTRS
    __tg_remquo(double __x, double __y, int* __z)
    {return remquo(__x, __y, __z);}

static long double
    _TGMATH_ATTRS
    __tg_remquo(long double __x,long double __y, int* __z)
    {return remquol(__x, __y, __z);}

#undef remquo
#define remquo(__x, __y, __z)                         \
        __tg_remquo(__tgmath_promote2((__x), (__y))(__x), \
                    __tgmath_promote2((__x), (__y))(__y), \
                    (__z))

/* rint */

static float
    _TGMATH_ATTRS
    __tg_rint(float __x) {return rintf(__x);}

static double
    _TGMATH_ATTRS
    __tg_rint(double __x) {return rint(__x);}

static long double
    _TGMATH_ATTRS
    __tg_rint(long double __x) {return rintl(__x);}

#undef rint
#define rint(__x) __tg_rint(__tgmath_promote1((__x))(__x))

/* round */

static float
    _TGMATH_ATTRS
    __tg_round(float __x) {return roundf(__x);}

static double
    _TGMATH_ATTRS
    __tg_round(double __x) {return round(__x);}

static long double
    _TGMATH_ATTRS
    __tg_round(long double __x) {return roundl(__x);}

#undef round
#define round(__x) __tg_round(__tgmath_promote1((__x))(__x))

/* scalbn */

static float
    _TGMATH_ATTRS
    __tg_scalbn(float __x, int __y) {return scalbnf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_scalbn(double __x, int __y) {return scalbn(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_scalbn(long double __x, int __y) {return scalbnl(__x, __y);}

#undef scalbn
#define scalbn(__x, __y) __tg_scalbn(__tgmath_promote1((__x))(__x), __y)

/* scalbln */

static float
    _TGMATH_ATTRS
    __tg_scalbln(float __x, long __y) {return scalblnf(__x, __y);}

static double
    _TGMATH_ATTRS
    __tg_scalbln(double __x, long __y) {return scalbln(__x, __y);}

static long double
    _TGMATH_ATTRS
    __tg_scalbln(long double __x, long __y) {return scalblnl(__x, __y);}

#undef scalbln
#define scalbln(__x, __y) __tg_scalbln(__tgmath_promote1((__x))(__x), __y)

/* tgamma */

static float
    _TGMATH_ATTRS
    __tg_tgamma(float __x) {return tgammaf(__x);}

static double
    _TGMATH_ATTRS
    __tg_tgamma(double __x) {return tgamma(__x);}

static long double
    _TGMATH_ATTRS
    __tg_tgamma(long double __x) {return tgammal(__x);}

#undef tgamma
#define tgamma(__x) __tg_tgamma(__tgmath_promote1((__x))(__x))

/* trunc */

static float
    _TGMATH_ATTRS
    __tg_trunc(float __x) {return truncf(__x);}

static double
    _TGMATH_ATTRS
    __tg_trunc(double __x) {return trunc(__x);}

static long double
    _TGMATH_ATTRS
    __tg_trunc(long double __x) {return truncl(__x);}

#undef trunc
#define trunc(__x) __tg_trunc(__tgmath_promote1((__x))(__x))

/* carg */

static float
    _TGMATH_ATTRS
    __tg_carg(float __x) {return atan2f(0.F, __x);}

static double
    _TGMATH_ATTRS
    __tg_carg(double __x) {return atan2(0., __x);}

static long double
    _TGMATH_ATTRS
    __tg_carg(long double __x) {return atan2l(0.L, __x);}

static float
    _TGMATH_ATTRS
    __tg_carg(float _Complex __x) {return cargf(__x);}

static double
    _TGMATH_ATTRS
    __tg_carg(double _Complex __x) {return carg(__x);}

static long double
    _TGMATH_ATTRS
    __tg_carg(long double _Complex __x) {return cargl(__x);}

#undef carg
#define carg(__x) __tg_carg(__tgmath_promote1((__x))(__x))

/* cimag */

static float
    _TGMATH_ATTRS
    __tg_cimag(float __x) {return 0;}

static double
    _TGMATH_ATTRS
    __tg_cimag(double __x) {return 0;}

static long double
    _TGMATH_ATTRS
    __tg_cimag(long double __x) {return 0;}

static float
    _TGMATH_ATTRS
    __tg_cimag(float _Complex __x) {return cimagf(__x);}

static double
    _TGMATH_ATTRS
    __tg_cimag(double _Complex __x) {return cimag(__x);}

static long double
    _TGMATH_ATTRS
    __tg_cimag(long double _Complex __x) {return cimagl(__x);}

#undef cimag
#define cimag(__x) __tg_cimag(__tgmath_promote1((__x))(__x))

/* conj */

static float _Complex
    _TGMATH_ATTRS
    __tg_conj(float __x) {return __x;}

static double _Complex
    _TGMATH_ATTRS
    __tg_conj(double __x) {return __x;}

static long double _Complex
    _TGMATH_ATTRS
    __tg_conj(long double __x) {return __x;}

static float _Complex
    _TGMATH_ATTRS
    __tg_conj(float _Complex __x) {return conjf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_conj(double _Complex __x) {return conj(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_conj(long double _Complex __x) {return conjl(__x);}

#undef conj
#define conj(__x) __tg_conj(__tgmath_promote1((__x))(__x))

/* cproj */

static float _Complex
    _TGMATH_ATTRS
    __tg_cproj(float __x) {return cprojf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_cproj(double __x) {return cproj(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_cproj(long double __x) {return cprojl(__x);}

static float _Complex
    _TGMATH_ATTRS
    __tg_cproj(float _Complex __x) {return cprojf(__x);}

static double _Complex
    _TGMATH_ATTRS
    __tg_cproj(double _Complex __x) {return cproj(__x);}

static long double _Complex
    _TGMATH_ATTRS
    __tg_cproj(long double _Complex __x) {return cprojl(__x);}

#undef cproj
#define cproj(__x) __tg_cproj(__tgmath_promote1((__x))(__x))

/* creal */

static float _Complex
    _TGMATH_ATTRS
    __tg_creal(float __x) {return __x;}

static double _Complex
    _TGMATH_ATTRS
    __tg_creal(double __x) {return __x;}

static long double _Complex
    _TGMATH_ATTRS
    __tg_creal(long double __x) {return __x;}

static float
    _TGMATH_ATTRS
    __tg_creal(float _Complex __x) {return crealf(__x);}

static double
    _TGMATH_ATTRS
    __tg_creal(double _Complex __x) {return creal(__x);}

static long double
    _TGMATH_ATTRS
    __tg_creal(long double _Complex __x) {return creall(__x);}

#undef creal
#define creal(__x) __tg_creal(__tgmath_promote1((__x))(__x))

#undef _TGMATH_ATTRSp
#undef _TGMATH_ATTRS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_TGMATH_H */

