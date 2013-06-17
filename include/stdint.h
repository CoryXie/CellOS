/* stdint.h - integer types */

/*
DESCRIPTION
 
    The <stdint.h> header shall declare sets of integer types having 
    specified widths, and shall define corresponding sets of macros.
    It shall also define macros that specify limits of integer types 
    corresponding to types defined in other standard headers.

    Note:
    
    The "width" of an integer type is the number of bits used to store 
    its value in a pure binary system; the actual type may use more bits
    than that (for example, a 28-bit type could be stored in 32 bits 
    of actual storage). An N-bit signed type has values in the range 
    -2N-1 or 1-2N-1 to 2N-1-1, while an N-bit unsigned type has values 
    in the range 0 to 2N-1.
    
    Types are defined in the following categories:

    - Integer types having certain exact widths

    - Integer types having at least certain specified widths

    - Fastest integer types having at least certain specified widths

    - Integer types wide enough to hold pointers to objects

    - Integer types having greatest width

    (Some of these types may denote the same type.)

    Corresponding macros specify limits of the declared types and 
    construct suitable constants.

    For each type described herein that the implementation provides, 
    the <stdint.h> header shall declare that typedef name and define 
    the associated macros. Conversely, for each type described herein 
    that the implementation does not provide, the <stdint.h> header 
    shall not declare that typedef name, nor shall it define the 
    associated macros. An implementation shall provide those types 
    described as required, but need not provide any of the others 
    (described as optional).

 RATIONALE
 
    The <stdint.h> header is a subset of the <inttypes.h> header more 
    suitable for use in freestanding environments, which might not
    support the formatted I/O functions. In some environments, if the 
    formatted conversion support is not wanted, using this header 
    instead of the <inttypes.h> header avoids defining such a large
    number of macros.
    
    As a consequence of adding int8_t, the following are true:
 
    - A byte is exactly 8 bits.
 
    - {CHAR_BIT} has the value 8, {SCHAR_MAX} has the value 127, 
      {SCHAR_MIN} has the value -128, and {UCHAR_MAX} has the value 255.
 
    (The POSIX standard explicitly requires 8-bit char and two's-complement arithmetic.)
 
 FUTURE DIRECTIONS
 
    typedef names beginning with int or uint and ending with _t may be 
    added to the types defined in the <stdint.h> header. Macro names 
    beginning with INT or UINT and ending with _MAX, _MIN, or _C may 
    be added to the macros defined in the <stdint.h> header.
 */
 
#ifndef _POSIX_STDINT_H
#define _POSIX_STDINT_H

/* Exact-width integer types */
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef signed long long int64_t;
typedef unsigned long long uint64_t;

/* Minimum-width integer types */
typedef int8_t int_least8_t;
typedef uint8_t uint_least8_t;

typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;

typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;

typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;

/* Fastest minimum-width integer types */
typedef int32_t int_fast8_t;
typedef uint32_t uint_fast8_t;

typedef int32_t int_fast16_t;
typedef uint32_t uint_fast16_t;

typedef int32_t int_fast32_t;
typedef uint32_t uint_fast32_t;

typedef int64_t int_fast64_t;
typedef uint64_t uint_fast64_t;

/* Greatest-width integer types */
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

/* Integer types capable of holding object pointers */
typedef signed long int intptr_t;
typedef unsigned long int uintptr_t;

/* Limits of exact-width integer types */
#define INT8_MIN 	(-128)
#define INT8_MAX 	(127)
#define UINT8_MAX	(255U)

#define INT16_MIN 	(-32768)
#define INT16_MAX 	(32767)
#define UINT16_MAX	(65535U)

#define INT32_MAX	(2147483647)
#define INT32_MIN 	(-INT32_MAX-1)
#define UINT32_MAX	(4294967295U)

#define INT64_MAX	(9223372036854775807LL)
#define INT64_MIN	(-INT64_MAX-1)
#define UINT64_MAX	(18446744073709551615ULL)

/* Limits of minimum-width integer types */
#define INT_LEAST8_MIN  	INT8_MIN
#define INT_LEAST8_MAX  	INT8_MAX
#define UINT_LEAST8_MAX 	UINT8_MAX

#define INT_LEAST16_MIN 	INT16_MIN
#define INT_LEAST16_MAX 	INT16_MAX
#define UINT_LEAST16_MAX	UINT16_MAX

#define INT_LEAST32_MIN 	INT32_MIN
#define INT_LEAST32_MAX 	INT32_MAX
#define UINT_LEAST32_MAX	UINT32_MAX

#define INT_LEAST64_MIN 	INT64_MIN
#define INT_LEAST64_MAX 	INT64_MAX
#define UINT_LEAST64_MAX	UINT64_MAX

/* Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN  	INT8_MIN
#define INT_FAST8_MAX  	INT8_MAX
#define UINT_FAST8_MAX 	UINT8_MAX

#define INT_FAST16_MIN 	INT16_MIN
#define INT_FAST16_MAX 	INT16_MAX
#define UINT_FAST16_MAX	UINT16_MAX

#define INT_FAST32_MIN 	INT32_MIN
#define INT_FAST32_MAX 	INT32_MAX
#define UINT_FAST32_MAX	UINT32_MAX

#define INT_FAST64_MIN 	INT64_MIN
#define INT_FAST64_MAX 	INT64_MAX
#define UINT_FAST64_MAX	UINT64_MAX

/* Limits of Integer types capable of holding object pointers */
#ifdef CONFIG_LP64
#define INTPTR_MIN	INT64_MIN
#define INTPTR_MAX	INT64_MAX
#define UINTPTR_MAX	UINT64_MAX
#else
#define INTPTR_MIN	INT32_MIN
#define INTPTR_MAX	INT32_MAX
#define UINTPTR_MAX	UINT32_MAX
#endif

/* Limits of Greatest-width integer types */
#define INTMAX_MIN	INT64_MIN
#define INTMAX_MAX	INT64_MAX
#define UINTMAX_MAX	UINT64_MAX

/* Limits of other integer types */
#ifdef CONFIG_LP64
#define PTDIFF_MIN INT64_MIN
#define PTDIFF_MAX INT64_MAX
#else
#define PTDIFF_MIN INT32_MIN
#define PTDIFF_MAX INT32_MAX
#endif

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

#define WCHAR_MIN INT32_MIN
#define WCHAR_MAX INT32_MAX

#ifdef CONFIG_LP64
#define SIZE_MAX 	UINT64_MAX
#else
#define SIZE_MAX 	UINT32_MAX
#endif

#define WINT_MIN 	0
#define WINT_MAX 	((wint_t)-1)

/* Macros of Integer Constant Expressions */
#define INT8_C(value) 	value
#define INT16_C(value) 	value
#define INT32_C(value) 	value
#define INT64_C(value) 	value ## LL

#define UINT8_C(value) 	value ## U
#define UINT16_C(value) value ## U
#define UINT32_C(value) value ## U
#define UINT64_C(value) value ## ULL

/* Macros for greatest-width integer constant expressions */
#define INTMAX_C(value) 	value ## LL
#define UINTMAX_C(value) 	value ## ULL

/* BSD compatibility */
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;


#endif	/* _POSIX_STDINT_H */
