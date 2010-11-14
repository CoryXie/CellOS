/* float.h - floating types */

/* REF: http://www.cwi.nl/~steven/enquire.html */

#ifndef _POSIX_ARCH_X64_FLOAT_H_
#define _POSIX_ARCH_X64_FLOAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Radix of exponent representation
 * 
 * Defines the base (radix) representation of the exponent 
 * (i.e. base-2 is binary, base-10 is the normal decimal 
 * representation, base-16 is Hex).
 */
 
#define FLT_RADIX       2

/*
 * Number of base-FLT_RADIX digits in the significand of a float 
 */
 
#define FLT_MANT_DIG    24

/*
 * Number of decimal digits of precision in a float 
 * 
 * The maximum number decimal digits (base-10) that can be 
 * represented without change after rounding.
 */
 
#define FLT_DIG         6

/* 
 * Defines the way floating-point numbers are rounded.
 *
 * -1   indeterminable
 * 0    toward zero
 * 1    to nearest
 * 2    toward positive infinity
 * 3    toward negative infinity
 */
 
#define FLT_ROUNDS      1

/* 
 * Difference between 1.0 and the minimum float greater than 1.0 
 */
 
#define FLT_EPSILON     1.19209290e-07F

/*
 * Minimum int x such that FLT_RADIX**(x-1) is a normalised float 
 *
 * The minimum negative integer value for an exponent in base 
 * FLT_RADIX.
 */
 
#define FLT_MIN_EXP     (-125)

/* Minimum normalised float */

#define FLT_MIN         1.17549435e-38F

/* Minimum int x such that 10**x is a normalised float */

#define FLT_MIN_10_EXP (-37)

/* Maximum int x such that FLT_RADIX**(x-1) is a representable float */

#define FLT_MAX_EXP     128

/* Maximum float */

#define FLT_MAX         3.40282347e+38F

/* Maximum int x such that 10**x is a representable float */

#define FLT_MAX_10_EXP  38

/* Number of base-FLT_RADIX digits in the significand of a double */

#define DBL_MANT_DIG    53

/* Number of decimal digits of precision in a double */

#define DBL_DIG         15

/* Difference between 1.0 and the minimum double greater than 1.0 */

#define DBL_EPSILON     2.2204460492503131e-16

/* Minimum int x such that FLT_RADIX**(x-1) is a normalised double */

#define DBL_MIN_EXP     (-1021)

/* Minimum normalised double */

#define DBL_MIN         2.2250738585072014e-308

/* Minimum int x such that 10**x is a normalised double */

#define DBL_MIN_10_EXP (-307)

/* 
 * Maximum int x such that FLT_RADIX**(x-1) is a representable double 
 */
 
#define DBL_MAX_EXP     1024

/* Maximum double */

#define DBL_MAX         1.7976931348623157e+308

/* Maximum int x such that 10**x is a representable double */

#define DBL_MAX_10_EXP  308

/* 
 * Number of base-FLT_RADIX digits in the significand of a long double
 */
 
#define LDBL_MANT_DIG   64

/* Number of decimal digits of precision in a long double */

#define LDBL_DIG        18

/* 
 * Difference between 1.0 and the minimum long double greater than 
 * 1.0 
 */
 
#define LDBL_EPSILON    1.08420217248550443401e-19L

/* 
 * Minimum int x such that FLT_RADIX**(x-1) is a normalised long 
 * double
 */
 
#define LDBL_MIN_EXP    (-16381)

/* Minimum normalised long double */

#define LDBL_MIN        3.36210314311209350626e-4932L

/* Minimum int x such that 10**x is a normalised long double */

#define LDBL_MIN_10_EXP (-4931)

/*
 * Maximum int x such that FLT_RADIX**(x-1) is a representable 
 * long double
 */
 
#define LDBL_MAX_EXP    16384

/* Maximum long double */

#define LDBL_MAX        1.18973149535723176502e+4932L

/* Maximum int x such that 10**x is a representable long double */

#define LDBL_MAX_10_EXP 4932

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_ARCH_X64_FLOAT_H_ */
